type t;

[@bs.module "next/router"] external router : t = "default";

[@bs.send] external replace : (t, string) => unit = "";