/* From https://github.com/ulrikstrid/bs-next */

[@bs.module "next/error"] external error : ReasonReact.reactClass = "default";
let make = (~statusCode: int, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=error,
    ~props={"statusCode": statusCode},
    children,
  );