let component = ReasonReact.reducerComponent("Login");

let default =
  ReasonNext.createPage(
    ~component,
    ~loader=_ => Js.Promise.resolve(),
    _props => (
      {
        ...component,
        didMount: _ => {
          let asPath = ReasonNext.Router.router->ReasonNext.Router.asPath;
          switch (asPath |> Js.String.split("#")) {
          | [|_, hash|] =>
            switch (hash |> Js.String.split("=")) {
            | [|"token", value|] =>
              ReasonNext.writeCookie(~name="token", value);
              ReasonNext.redirect("/");
            | _ => ()
            }
          | _ => ()
          };
        },
        render: _self => ReasonReact.string("Redirecting..."),
      }:
        ReasonReact.component(unit, 'a, unit)
    ),
  );
