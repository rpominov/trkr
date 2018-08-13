let component = ReasonReact.reducerComponent("Login");

let default =
  Next.Page.create(
    ~component,
    ~loader=_ => Js.Promise.resolve(),
    props => (
      {
        ...component,
        didMount: _ => {
          let asPath = props->Next.Page.Props.routerGet->Next.Router.asPathGet;
          switch (asPath |> Js.String.split("#")) {
          | [|_, hash|] =>
            switch (hash |> Js.String.split("=")) {
            | [|"token", value|] =>
              Next.writeCookie("token", value);
              Next.redirect("/");
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
