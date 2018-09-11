module Process = {
  [@bs.deriving abstract]
  type t = pri {browser: Js.Nullable.t(bool)};

  [@bs.val] external process: t = "";

  let inBrowser =
    switch (process |> browserGet |> Js.Nullable.toOption) {
    | Some(x) => x
    | None => false
    };
};

module LocalStorage = {
  type t;

  [@bs.send] external getItem: (t, string) => Js.Nullable.t(string) = "";
  [@bs.send] external setItem: (t, string, string) => unit = "";

  [@bs.val] external localStorage: t = "";
};

type t = Js.Dict.t(TimeRecord.t);

let localStorageKey = "untrackedTime";

let readLocalStorage = (): t =>
  if (!Process.inBrowser) {
    Js.Dict.empty();
  } else {
    switch (
      LocalStorage.(getItem(localStorage, localStorageKey))
      |> Js.Nullable.toOption
    ) {
    | Some(data) =>
      try (data |> Json.parseOrRaise |> Json.Decode.(int |> dict |> dict)) {
      | _ => Js.Dict.empty()
      }
    | None => Js.Dict.empty()
    };
  };

let writeLocalStorage = (data: t): unit =>
  if (!Process.inBrowser) {
    ();
  } else {
    switch (Js.Json.stringifyAny(data)) {
    | Some(str) => LocalStorage.(setItem(localStorage, localStorageKey, str))
    | None => ()
    };
  };

let removeLocalRecord = (~cardId: string) =>
  readLocalStorage()
  |> Js.Dict.entries
  |> Js.Array.filter(((key, _)) => key !== cardId)
  |> Js.Dict.fromArray
  |> writeLocalStorage;

let getRecords = (cards: array(Trello.Card.t), ~fieldId: string): t => {
  let localData = readLocalStorage();
  let result = Js.Dict.empty();

  cards
  |> Js.Array.forEach(card => {
       let {Trello.Card.id} = card;
       TimeRecord.(
         switch (
           Trello.Card.getCustomField(~fieldId, card),
           Js.Dict.get(localData, id),
         ) {
         | (Some(a), b) => combine(parse(a), b)
         | (None, Some(b)) => b
         | (None, None) => empty()
         }
       )
       |> Js.Dict.set(result, id);
     });

  result;
};

let totalUntracked = () =>
  readLocalStorage()
  |> Js.Dict.values
  |> TimeRecord.combineAll
  |> TimeRecord.aggregateTime;

let fetchTrkrFieldId = (~boardId: string): Trello.Monad.t(string) =>
  Trello.Monad.(
    Trello.fetchBoard(boardId)
    |> flatMap(board =>
         switch (board |> Trello.Board.getTrkrFieldId) {
         | Some(id) => pure(id)
         | None =>
           error(
             Trello.CustomFailure(
               "The Trello board \""
               ++ board.Trello.Board.name
               ++ "\" doensn't have a \"trkr\" custom field.",
             ),
           )
         }
       )
  );

let track' =
    (~fieldId: option(string)=?, ~cardId: string, append: TimeRecord.t) =>
  None
  |> Trello.Monad.(
       Trello.fetchCard(cardId)
       |> flatMap(card =>
            (
              switch (fieldId) {
              | Some(x) => pure(x)
              | None => fetchTrkrFieldId(~boardId=card.Trello.Card.idBoard)
              }
            )
            |> flatMap(fieldId =>
                 Trello.setCustomField(
                   ~cardId=card.Trello.Card.id,
                   ~fieldId,
                   ~newValue=
                     TimeRecord.stringify(
                       Trello.Card.getTimeRecord(~fieldId, card)
                       |> TimeRecord.combine(append),
                     ),
                 )
               )
          )
     );

let track = (~fieldId: option(string)=?, ~cardId: string, timeAmmount: int) => {
  let append = TimeRecord.increment(TimeRecord.empty(), timeAmmount);
  track'(~cardId, ~fieldId?, append)
  |> PromiseResult.recover(_ => {
       let localData = readLocalStorage();
       Js.Dict.set(
         localData,
         cardId,
         Js.Dict.get(localData, cardId) |> TimeRecord.combine(append),
       );
       writeLocalStorage(localData);
     })
  |> PromiseResult.map(_ => append);
};

let reSend = () =>
  PromiseResult.(
    readLocalStorage()
    |> Js.Dict.entries
    |> iterate(((cardId, record)) =>
         track'(~cardId, record) |> map(_ => removeLocalRecord(~cardId))
       )
    |> map(_ => ())
  );
