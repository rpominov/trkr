/* From https://github.com/ulrikstrid/bs-next */

[@bs.module "next/head"] external head: ReasonReact.reactClass = "default";
let make = children =>
  ReasonReact.wrapJsForReason(
    ~reactClass=head,
    ~props=Js.Obj.empty(),
    children,
  );
