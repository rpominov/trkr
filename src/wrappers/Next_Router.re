[@bs.deriving abstract]
type t =
  pri {
    asPath: string,
    mutable onRouteChangeStart: Js.Nullable.t(unit => unit),
    mutable onRouteChangeComplete: Js.Nullable.t(unit => unit),
    mutable onRouteChangeError: Js.Nullable.t(unit => unit),
  };

[@bs.module "next/router"] external router: t = "default";

[@bs.send] external replace: (t, string) => unit = "";
