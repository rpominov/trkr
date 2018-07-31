let css: {. "box": string} = [%raw {|require('./ErrorPage.css')|}];

let component = ReasonReact.statelessComponent("ErrorPage");

let make = (~error: Trello.failure, _children) => {
  ...component,
  render: _self => {
    let content =
      Trello.(
        switch (error) {
        | AppKeyIsNotSet => "The APP_KEY enviroment variable is not set."
        | Unauthenticated => "You need to log in."
        | NetworkProblem => "Network problem."
        | BadStatus(s) =>
          "Bad API response (status " ++ string_of_int(s) ++ ")."
        | BadResponseBody(str) => "Bad API response.\n\n" ++ str
        | CustomFailure(str) => str
        }
      );

    <div className=css##box>
      <h2> (ReasonReact.string("Error")) </h2>
      <pre> (ReasonReact.string(content)) </pre>
    </div>;
  },
};

let renderError = (props, regularRender) => {
  let data = props |. Next.Page.Props.data;

  switch (data) {
  | Belt.Result.Ok(data') => regularRender(data')
  | Belt.Result.Error(error) => make(~error, [||]) |> ReasonReact.element
  };
};