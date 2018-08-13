[@bs.deriving abstract]
type t = pri {publicRuntimeConfig: Js.Dict.t(string)};

[@bs.module "next/config"] external getConfig: unit => t = "default";
