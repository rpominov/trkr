module LoadingContext = Next_LoadingContext;
module Link = Next_Link;
module Head = Next_Head;
module Error = Next_Error;
module Page = Next_Page;
module Router = Next_Router;
module Config = Next_Config;

module Cookie = {
  [@bs.deriving abstract]
  type options = {maxAge: Js.Undefined.t(int)};

  [@bs.module "cookie"]
  external serialize: (string, string, options) => string = "";

  [@bs.module "cookie"] external parse: string => Js.Dict.t(string) = "";
};

module Document = {
  [@bs.deriving abstract]
  type document = pri {mutable cookie: string};

  [@bs.val] external document: document = "";
};

module Location = {
  [@bs.deriving abstract]
  type location =
    pri {
      protocol: string,
      host: string,
    };

  [@bs.val] external location: location = "";
};

let writeCookie =
    (
      ~context: option(LoadingContext.t)=?,
      ~maxAge: int=60 * 60 * 24 * 356,
      name: string,
      value: string,
    ) => {
  let options = Cookie.options(~maxAge=Js.Undefined.return(maxAge));
  let cookieString = Cookie.serialize(name, value, options);

  switch (
    switch (context) {
    | Some(ctx) => ctx->LoadingContext.resGet->Js.Nullable.toOption
    | None => None
    }
  ) {
  | Some(res) => LoadingContext.Res.setHeader(res, "Set-Cookie", cookieString)
  | None => Document.(cookieSet(document, cookieString))
  };
};

let readCookie =
    (~context: option(LoadingContext.t)=?, ()): Js.Dict.t(string) =>
  Cookie.parse(
    switch (
      switch (context) {
      | Some(ctx) => ctx->LoadingContext.reqGet->Js.Nullable.toOption
      | None => None
      }
    ) {
    | Some(req) =>
      switch (Js.Dict.get(req->LoadingContext.Req.headersGet, "cookie")) {
      | Some(s) => s
      | None => ""
      }
    | None => Document.(document->cookieGet)
    },
  );

let redirect = (~context: option(LoadingContext.t)=?, target: string) =>
  switch (
    switch (context) {
    | Some(ctx) => ctx->LoadingContext.resGet->Js.Nullable.toOption
    | None => None
    }
  ) {
  | Some(res) =>
    let headers = Js.Dict.fromArray([|("Location", target)|]);
    LoadingContext.Res.writeHead(res, 303, headers);
    LoadingContext.Res.end_(res);
  | None => Router.replace(Router.router, target)
  };

let setResponseStatusCode =
    (~context: option(LoadingContext.t)=?, status: int) =>
  switch (
    switch (context) {
    | Some(ctx) => ctx->LoadingContext.resGet->Js.Nullable.toOption
    | None => None
    }
  ) {
  | Some(res) => LoadingContext.Res.statusCodeSet(res, status)
  | None => ()
  };

let getBaseUrl = (~context: option(LoadingContext.t)=?, ()) =>
  switch (
    switch (context) {
    | Some(ctx) => ctx->LoadingContext.reqGet->Js.Nullable.toOption
    | None => None
    }
  ) {
  | Some(req') =>
    LoadingContext.Req.(
      (isSecure(req') ? "https" : "http")
      ++ "://"
      ++ Js.Dict.unsafeGet(req'->headersGet, "host")
    )
  | None => Location.(location->protocolGet ++ "//" ++ location->hostGet)
  };
