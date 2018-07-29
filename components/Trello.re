[%raw "require('isomorphic-fetch')"];

let appKey =
  Js.Dict.get(Next.Config.(getConfig() |. publicRuntimeConfig), "appKey");

type failure =
  | AppKeyIsNotSet
  | Unauthenticated
  | NetworkProblem
  | BadStatus(int)
  | BadResponseBody(string)
  | CustomFailure(string);

let fetch =
    (
      ~body: option(string)=?,
      ~params: Js.Array.t((string, string)),
      ~method: Fetch.requestMethod=Fetch.Get,
      ~decoder: Js.Json.t => 'a,
      path: string,
      context: option(Next.LoadingContext.t),
    )
    : PromiseResult.t('a, failure) => {
  let token = Js.Dict.get(Next.readCookie(~context?, ()), "token");

  switch (token, appKey) {
  | (_, None) => PromiseResult.error(AppKeyIsNotSet)
  | (None, _) => PromiseResult.error(Unauthenticated)
  | (Some(token), Some(appKey)) =>
    let params' =
      Js.Array.(
        params
        |> concat([|("key", appKey), ("token", token)|])
        |> map(((key, value)) => key ++ "=" ++ value)
        |> joinWith("&")
      );

    let url = "https://api.trello.com/1/" ++ path ++ "?" ++ params';

    let requestInit =
      switch (body) {
      | Some(body) =>
        Fetch.RequestInit.make(
          ~method_=method,
          ~body=Fetch.BodyInit.make(body),
          ~headers=
            Fetch.HeadersInit.make({"Content-Type": "application/json"}),
          (),
        )
      | None => Fetch.RequestInit.make(~method_=method, ())
      };

    let result =
      PromiseResult.(
        Fetch.fetchWithInit(url, requestInit)
        |> wrap(_ => NetworkProblem)
        |> flatMap(resp =>
             switch (resp |. Fetch.Response.status) {
             | 200 => resp |. Fetch.Response.text |> wrap(_ => NetworkProblem)
             | 401 => error(Unauthenticated)
             | s => error(BadStatus(s))
             }
           )
        |> tryMap(
             text => text |> Json.parseOrRaise |> decoder,
             (e, text) =>
               switch (e) {
               | Json.Decode.DecodeError(str) => BadResponseBody(str)
               | Json.ParseError(str) => BadResponseBody(str)
               | _ => BadResponseBody(text)
               },
           )
      );

    result;
  };
};

module Monad = {
  type apiCall('a) =
    option(Next.LoadingContext.t) => PromiseResult.t('a, failure);

  let pure = (x: 'a) : apiCall('a) => _ => PromiseResult.ok(x);

  let map = (f: 'a => 'b, call: apiCall('a)) : apiCall('b) =>
    ctx => call(ctx) |> PromiseResult.map(f);

  let flatMap = (f: 'a => apiCall('b), call: apiCall('a)) : apiCall('b) =>
    ctx => call(ctx) |> PromiseResult.flatMap(x => f(x, ctx));

  /* TODO: handle Unauthenticated here */
  let makeLoader = (call: apiCall('a), context) => call(Some(context));
};

module Board = {
  module Prefs = {
    type t = {
      backgroundBrightness: string,
      backgroundImage: option(string),
      backgroundTopColor: string,
    };

    let decoder =
      Json.Decode.(
        x => {
          backgroundBrightness: x |> field("backgroundBrightness") @@ string,
          backgroundImage:
            x |> optional @@ field("backgroundImage") @@ string,
          backgroundTopColor: x |> field("backgroundTopColor") @@ string,
        }
      );
  };

  module CustomField = {
    type t = {
      name: string,
      id: string,
    };

    let decoder =
      Json.Decode.(
        x => {
          name: x |> field("name") @@ string,
          id: x |> field("id") @@ string,
        }
      );
  };

  type t = {
    name: string,
    id: string,
    prefs: Prefs.t,
    customFields: option(array(CustomField.t)),
  };

  let requestParams = [|("fields", "name,id,prefs")|];

  let decoder =
    Json.Decode.(
      x => {
        name: x |> field("name") @@ string,
        id: x |> field("id") @@ string,
        prefs: x |> field("prefs") @@ Prefs.decoder,
        customFields:
          x
          |> optional @@
          field("customFields") @@
          array(CustomField.decoder),
      }
    );

  let getTrkrFieldId = board =>
    Js.Option.(
      board.customFields
      |> andThen((. fields) =>
           fields
           |> Js.Array.find(f => f.CustomField.name === "trkr")
           |> map((. f) => f.CustomField.id)
         )
    );
};

module Card = {
  module CustomFieldItem = {
    type t = {
      id: string,
      idCustomField: string,
      value: string,
    };

    let decoder =
      Json.Decode.(
        x => {
          id: x |> field("id") @@ string,
          idCustomField: x |> field("idCustomField") @@ string,
          value: x |> field("value") @@ field("text") @@ string,
        }
      );

    let encode = value =>
      Json.Encode.(
        object_([("value", object_([("text", string(value))]))])
      )
      |> Json.stringify;
  };

  type t = {
    id: string,
    name: string,
    idBoard: string,
    shortUrl: string,
    idList: string,
    customFieldItems: array(CustomFieldItem.t),
  };

  let requestParams = [|
    ("customFieldItems", "true"),
    ("fields", "id,name,idBoard,shortUrl,idList"),
  |];

  let decoder =
    Json.Decode.(
      x => {
        id: x |> field("id") @@ string,
        name: x |> field("name") @@ string,
        idBoard: x |> field("idBoard") @@ string,
        shortUrl: x |> field("shortUrl") @@ string,
        idList: x |> field("idList") @@ string,
        customFieldItems:
          x |> field("customFieldItems") @@ array(CustomFieldItem.decoder),
      }
    );

  let getCustomField = (~fieldId: string, card: t) : option(string) =>
    card.customFieldItems
    |> Js.Array.find(x => x.CustomFieldItem.idCustomField === fieldId)
    |> Js.Option.map((. x) => x.CustomFieldItem.value);
};

module List = {
  type t = {name: string};

  let requestParams = [|("fields", "name")|];

  let decoder = Json.Decode.(x => {name: x |> field("name") @@ string});
};

let fetchMyBoards = () =>
  fetch(
    ~decoder=Json.Decode.(array(Board.decoder)),
    ~params=Js.Array.concat([|("filter", "starred")|], Board.requestParams),
    "members/me/boards",
  );

let fetchBoard = (boardId: string) =>
  fetch(
    ~decoder=Board.decoder,
    ~params=
      Js.Array.concat([|("customFields", "true")|], Board.requestParams),
    "boards/" ++ boardId,
  );

let fetchLists = (boardId: string) =>
  fetch(
    ~decoder=Json.Decode.(array(List.decoder)),
    ~params=List.requestParams,
    "boards/" ++ boardId ++ "/lists",
  );

let fetchCards = (boardId: string) =>
  fetch(
    ~decoder=Json.Decode.(array(Card.decoder)),
    ~params=Card.requestParams,
    "boards/" ++ boardId ++ "/cards/open",
  );

let fetchCard = (cardId: string) =>
  fetch(
    ~decoder=Card.decoder,
    ~params=Card.requestParams,
    "cards/" ++ cardId,
  );

let setCustomField = (~cardId: string, ~fieldId: string, ~newValue: string) =>
  fetch(
    ~decoder=_ => (),
    ~params=[||],
    ~method=Fetch.Put,
    ~body=Card.CustomFieldItem.encode(newValue),
    "cards/" ++ cardId ++ "/customField/" ++ fieldId ++ "/item",
  );