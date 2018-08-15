let css: {. "box": string} = [%raw {|require('components/ErrorPage.css')|}];

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
        | NotFound => "404 Not Found"
        }
      );

    <div className=css##box>
      <h2> {ReasonReact.string("Error")} </h2>
      <pre> {ReasonReact.string(content)} </pre>
    </div>;
  },
};

let renderError = (data, regularRender) =>
  switch (data) {
  | Belt.Result.Ok(data') => regularRender(data')
  | Belt.Result.Error(error) => make(~error, [||]) |> ReasonReact.element
  };