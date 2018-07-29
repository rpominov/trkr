let component = ReasonReact.statelessComponent("Test");

type data = {
  pathname: string,
  queryParam: option(string),
};

let loader = context =>
  Next.LoadingContext.(
    Js.Promise.resolve({
      pathname: context |. pathname,
      queryParam: Js.Dict.get(context |. query, "param"),
    })
  );

let make = (~props, _children) => {
  ...component,
  render: _self => {
    let data = props |. Next.Page.Props.data;
    let pathname = data.pathname;
    let queryParam = Js.Option.getWithDefault("None", data.queryParam);

    let content = {j|

pathname: $pathname
queryParam: $queryParam

   |j};

    <pre> (content |> Js.String.trim |> ReasonReact.string) </pre>;
  },
};

let default =
  Next.Page.create(~component, ~loader, props => make(~props, [||]));