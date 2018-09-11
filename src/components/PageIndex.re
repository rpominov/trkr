let css: {. "boardItem": string} = [%raw
  {|require('components/PageIndex.css')|}
];

let renderBoard = (board: Trello.Board.t) => {
  let color = board.prefs.backgroundBrightness === "dark" ? "white" : "black";

  let backgroundImage =
    switch (board.prefs.backgroundImage) {
    | Some(url) => "url(" ++ url ++ ")"
    | None => ""
    };

  let backgroundColor = board.prefs.backgroundTopColor;

  let style =
    ReactDOMRe.Style.make(~color, ~backgroundImage, ~backgroundColor, ());

  let href = {
    "pathname": "/board",
    "query": {
      "id": board.id,
    },
  };

  <li key={board.id} className=css##boardItem>
    <ReasonNext.Link href>
      <a style> {board.name |> ReasonReact.string} </a>
    </ReasonNext.Link>
  </li>;
};

type data = array(Trello.Board.t);

let component = ReasonReact.statelessComponent("BoardsPage");

let loader = Trello.fetchMyBoards() |> Trello.makeLoader;

let default =
  ReasonNext.createPage(~component, ~loader, props =>
    {
      ...component,
      render: _self =>
        ErrorPage.renderError(props, (boards: data) =>
          <ol>
            {boards |> Js.Array.map(renderBoard) |> ReasonReact.array}
          </ol>
        ),
    }
  );
