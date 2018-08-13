/* From https://github.com/ulrikstrid/bs-next */

[@bs.module "next/link"] external link: ReasonReact.reactClass = "default";
let make =
    (
      ~href=?,
      ~_as=?,
      ~prefetch: option(bool)=?,
      ~replace: option(bool)=?,
      ~shallow: option(bool)=?,
      ~passHref: option(bool)=?,
      children: array(ReasonReact.reactElement),
    ) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=link,
    ~props=
      Js.Undefined.{
        "href": fromOption(href),
        "as": fromOption(_as),
        "prefetch": fromOption(prefetch),
        "replace": fromOption(replace),
        "shallow": fromOption(shallow),
        "passHref": fromOption(passHref),
      },
    children,
  );
