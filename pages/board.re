let css: {
  .
  "wrap": string,
  "time": string,
  "listWrap": string,
  "toggleCollapse": string,
} = [%raw
  {|require('./board.css')|}
];

let incrementBy = 10;
let collapsedListsCookieName = "coli";

module CollapsedLists = {
  type t =
    | CollapsedLists(array(string));

  let parse = maybeStr : t =>
    (
      switch (maybeStr) {
      | Some(str) => Js.String.split("-", str)
      | None => [||]
      }
    )
    |. CollapsedLists;

  let stringify = (CollapsedLists(lists)) => Js.Array.joinWith("-", lists);

  let isCollapsed = (CollapsedLists(lists), id) =>
    lists |> Js.Array.includes(id);

  let toggle = (CollapsedLists(lists) as lists', id) =>
    isCollapsed(lists', id) ?
      CollapsedLists(Js.Array.filter(x => x !== id, lists)) :
      CollapsedLists(Js.Array.concat(lists, [|id|]));
};

module Board = {
  type state = {
    currentCard: option(Trello.Card.t),
    records: Storage.t,
    collapsedLists: CollapsedLists.t,
  };

  type action =
    | IncrementCurrent
    | SetCurrentCard(option(Trello.Card.t))
    | IncrementDone(string, TimeRecord.t)
    | ToggleCollapsed(string);

  let renderHeader = (board: Trello.Board.t) => {
    let (color, textBackground) =
      board.prefs.backgroundBrightness === "dark" ?
        ("white", "rgba(0,0,0,0.5)") : ("black", "rgba(255,255,255,0.5)");

    let backgroundImage =
      switch (board.prefs.backgroundImage) {
      | Some(url) => "url(" ++ url ++ ")"
      | None => ""
      };

    let style =
      ReactDOMRe.Style.make(
        ~backgroundImage,
        ~backgroundColor=board.prefs.backgroundTopColor,
        (),
      );

    let style' =
      ReactDOMRe.Style.make(~color, ~backgroundColor=textBackground, ());

    <h2>
      <div style />
      <span style=style'> (board.name |> ReasonReact.string) </span>
    </h2>;
  };

  let renderCard = (~isCurrent, ~record, ~onSelect, card: Trello.Card.t) => {
    let elementId = "current-card-radio__" ++ card.id;

    let time = record |> TimeRecord.formatTodayRest(~blink=isCurrent);

    <li key=card.id>
      <input
        _type="radio"
        name="current-card"
        checked=isCurrent
        onChange=onSelect
        id=elementId
      />
      <div>
        <label htmlFor=elementId> (card.name |> ReasonReact.string) </label>
        <p className=css##time> time </p>
      </div>
    </li>;
  };

  let component = ReasonReact.reducerComponent("Board");

  let make =
      (
        ~fieldId: string,
        ~board: Trello.Board.t,
        ~lists: array(Trello.List.t),
        ~cards: array(Trello.Card.t),
        ~collapsedLists: CollapsedLists.t,
        _children,
      ) => {
    ...component,
    initialState: () => {
      currentCard: None,
      records: Storage.getRecords(cards, ~fieldId),
      collapsedLists,
    },
    reducer: (action: action, state: state) =>
      switch (action) {
      | IncrementCurrent =>
        SideEffects(
          (
            self =>
              switch (self.state.currentCard) {
              | Some(card) =>
                Storage.track(~fieldId, ~cardId=card.id, incrementBy)
                |> PromiseResult.consume(append =>
                     self.send(IncrementDone(card.id, append))
                   )
              | None => ()
              }
          ),
        )
      | IncrementDone(cardId, append) =>
        Update({
          ...state,
          records:
            Util.dictAssoc(
              state.records,
              cardId,
              Js.Dict.get(state.records, cardId)
              |> TimeRecord.combine(append),
            ),
        })
      | SetCurrentCard(currentCard) => Update({...state, currentCard})
      | ToggleCollapsed(listId) =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            collapsedLists:
              CollapsedLists.toggle(state.collapsedLists, listId),
          },
          (
            self =>
              Next.writeCookie(
                collapsedListsCookieName,
                self.state.collapsedLists |> CollapsedLists.stringify,
              )
          ),
        )
      },
    didMount: self => {
      let cb = () => self.send(IncrementCurrent);
      let id = Js.Global.setInterval(cb, incrementBy * 1000);
      self.onUnmount(() => Js.Global.clearInterval(id));
    },
    render: self => {
      let title =
        (
          switch (self.state.currentCard) {
          | None => "Resting..."
          | Some(card) =>
            (
              Js.Dict.unsafeGet(self.state.records, card.id)
              |> TimeRecord.formatToday
            )
            ++ "h @ "
            ++ card.name
          }
        )
        |> ReasonReact.string;

      let totalTime =
        self.state.records
        |> Js.Dict.values
        |> TimeRecord.combineAll
        |> TimeRecord.formatTodayRest;

      let isResting = self.state.currentCard |> Js.Option.isNone;
      let onStartResting = _ => self.send(SetCurrentCard(None));

      <div className=css##wrap>
        <Favicon animated=(! isResting) />
        <Next.Head> <title> title </title> </Next.Head>
        (renderHeader(board))
        <p className=css##time> totalTime </p>
        <ul>
          <li>
            <input
              _type="radio"
              name="current-card"
              id="current-card-radio__rest"
              checked=isResting
              onChange=onStartResting
            />
            <div>
              <label htmlFor="current-card-radio__rest">
                ("Rest..." |> ReasonReact.string)
              </label>
            </div>
          </li>
        </ul>
        (
          lists
          |> Js.Array.map((list: Trello.List.t) => {
               let isCollapsed =
                 CollapsedLists.isCollapsed(
                   self.state.collapsedLists,
                   list.id,
                 );

               let toggleCollapse = _ => self.send(ToggleCollapsed(list.id));

               <div key=list.id className=css##listWrap>
                 <h3>
                   (list.name |> ReasonReact.string)
                   <button
                     className=css##toggleCollapse onClick=toggleCollapse>
                     (
                       ReasonReact.string(
                         isCollapsed ? "[expand]" : "[collapse]",
                       )
                     )
                   </button>
                 </h3>
                 (
                   isCollapsed ?
                     ReasonReact.null :
                     cards
                     |> Js.Array.filter(card =>
                          card.Trello.Card.idList === list.id
                        )
                     |> Js.Array.map(card =>
                          renderCard(
                            ~isCurrent=
                              switch (self.state.currentCard) {
                              | Some(x) => x === card
                              | None => false
                              },
                            ~record=
                              Js.Dict.unsafeGet(self.state.records, card.id),
                            ~onSelect=
                              _ => self.send(SetCurrentCard(Some(card))),
                            card,
                          )
                        )
                     |> ReasonReact.array
                 )
               </div>;
             })
          |> ReasonReact.array
        )
      </div>;
    },
  };

  let renderUninteractive = board =>
    <div className=css##wrap>
      (renderHeader(board))
      (
        "Create the \"trkr\" custom field in the board before you can use it with TRKR."
        |> ReasonReact.string
      )
    </div>;
};

type data =
  | WithTrkrField(
      string,
      Trello.Board.t,
      Js.Array.t(Trello.List.t),
      array(Trello.Card.t),
      CollapsedLists.t,
    )
  | WithoutTrkrField(Trello.Board.t);

let loader =
  Trello.Monad.(
    Trello.getQueryParam("id")
    |> flatMap(boardId =>
         Trello.fetchBoard(boardId)
         |> flatMap(board =>
              switch (Trello.Board.getTrkrFieldId(board)) {
              | None => pure(WithoutTrkrField(board))
              | Some(fieldId) =>
                Trello.fetchLists(boardId)
                |> map(Js.Array.reverseInPlace)
                |> flatMap(lists =>
                     Trello.fetchCards(boardId)
                     |> flatMap(cards =>
                          Trello.getCookie(collapsedListsCookieName)
                          |> map(CollapsedLists.parse)
                          |> map(collapsedLists =>
                               WithTrkrField(
                                 fieldId,
                                 board,
                                 lists,
                                 cards,
                                 collapsedLists,
                               )
                             )
                        )
                   )
              }
            )
       )
    |> Trello.makeLoader
  );

let component = ReasonReact.statelessComponent("BoardPage");

let default =
  Next.Page.create(~component, ~loader, props =>
    {
      ...component,
      render: _self =>
        ErrorPage.renderError(props, data =>
          switch (data) {
          | WithoutTrkrField(board) => Board.renderUninteractive(board)
          | WithTrkrField(fieldId, board, lists, cards, collapsedLists) =>
            <Board fieldId board lists cards collapsedLists />
          }
        ),
    }
  );