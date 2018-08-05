module Req = {
  module Connection = {
    type secure;

    [@bs.deriving abstract]
    type t = pri {secure: Js.Nullable.t(secure)};
  };

  [@bs.deriving abstract]
  type t =
    pri {
      headers: Js.Dict.t(string),
      connection: Connection.t,
    };

  let isSecure = req =>
    switch (req |. connection |. Connection.secure |. Js.Nullable.toOption) {
    | Some(_) => true
    | None => false
    };
};

module Res = {
  [@bs.deriving abstract]
  type t =
    pri {
      finished: bool,
      headersSent: bool,
      mutable statusCode: int,
    };

  [@bs.send] external end_ : t => unit = "end";
  [@bs.send] external hasHeader : (t, string) => bool = "";
  [@bs.send] external getHeader : (t, string) => Js.Nullable.t(string) = "";
  [@bs.send] external setHeader : (t, string, string) => unit = "";
  [@bs.send] external removeHeader : (t, string) => unit = "";
  [@bs.send] external writeHead : (t, int, Js.Dict.t(string)) => unit = "";
};

module BrowserRes = {
  type t;
};

module Err = {
  type t;
};

[@bs.deriving abstract]
type t =
  pri {
    /* path section of URL */
    pathname: string,
    /* query string section of URL parsed as an object */
    query: Js.Dict.t(string),
    /* String of the actual path (including the query) shows in the browser */
    asPath: string,
    /* HTTP request object (server only) */
    req: Js.nullable(Req.t),
    /* HTTP response object (server only) */
    res: Js.nullable(Res.t),
    /* Fetch Response object (client only) */
    jsonPageRes: Js.nullable(BrowserRes.t),
    /* Error object if any error is encountered during the rendering */
    err: Js.nullable(Err.t),
  };