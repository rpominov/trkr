let css: {. "boardItem": string} = [%raw {|require('./index.css')|}];

let renderBoard = (board: Trello.Board.t) => {
  module B = Trello.Board;
  module P = Trello.Board.Prefs;

  let {
    B.prefs: {
      P.backgroundBrightness,
      P.backgroundImage,
      P.backgroundTopColor,
    },
    B.id,
    B.name,
  } = board;

  let color = backgroundBrightness === "dark" ? "white" : "black";

  let backgroundImage =
    switch (backgroundImage) {
    | Some(url) => "url(" ++ url ++ ")"
    | None => ""
    };

  let backgroundColor = backgroundTopColor;

  let style =
    ReactDOMRe.Style.make(~color, ~backgroundImage, ~backgroundColor, ());

  let href = {
    "pathname": "/board",
    "query": {
      "id": id,
    },
  };

  <li key=id className=css##boardItem>
    <Next.Link href> <a style> (name |> ReasonReact.string) </a> </Next.Link>
  </li>;
};

type data = {boards: array(Trello.Board.t)};

let render = (data: data) =>
  <ol> (data.boards |> Js.Array.map(renderBoard) |> ReasonReact.array) </ol>;

let component = ReasonReact.statelessComponent("BoardsPage");

let loader =
  Trello.fetchMyBoards()
  |> Trello.Monad.map(boards => {boards: boards})
  |> Trello.Monad.makeLoader;

let default =
  Next.Page.create(~component, ~loader, props =>
    {...component, render: _self => Util.renderError(props, render)}
  );