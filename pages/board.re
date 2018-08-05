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

let renderUninteractive = board =>
  <div className=css##wrap>
    (renderHeader(board))
    (
      "Create the \"trkr\" custom field in the board before you can use it with TRKR."
      |> ReasonReact.string
    )
  </div>;

module InteractiveBoard = {
  type state = {
    currentCard: option(Trello.Card.t),
    records: Storage.t,
    collapsedLists: array(string),
  };

  type action =
    | SetCurrentCard(option(Trello.Card.t))
    | UpdateRecord(string, TimeRecord.t)
    | SetCollapsed(string, bool);

  let component = ReasonReact.reducerComponent("InteractiveBoard");

  let make =
      (
        ~fieldId: string,
        ~board: Trello.Board.t,
        ~lists: array(Trello.List.t),
        ~cards: array(Trello.Card.t),
        ~initialCollapsedLists: array(string),
        _children,
      ) => {
    ...component,
    initialState: () => {
      currentCard: None,
      records: Storage.getRecords(cards, ~fieldId),
      collapsedLists: initialCollapsedLists,
    },
    didMount: self => {
      let id =
        Js.Global.setInterval(
          self.handle((_, self) =>
            switch (self.state.currentCard) {
            | Some(card) =>
              ignore(
                Storage.track(~fieldId, ~cardId=card.id, incrementBy)
                |> PromiseResult.map(
                     self.handle((append, self) =>
                       self.send(
                         UpdateRecord(
                           card.id,
                           Js.Dict.get(self.state.records, card.id)
                           |> TimeRecord.combine(append),
                         ),
                       )
                     ),
                   ),
              )
            | None => ()
            }
          ),
          incrementBy * 1000,
        );
      self.onUnmount(() => Js.Global.clearInterval(id));
    },
    reducer: (action: action, state: state) =>
      switch (action) {
      | UpdateRecord(cardId, record) =>
        Update({
          ...state,
          records: Util.dictAssoc(state.records, cardId, record),
        })
      | SetCurrentCard(currentCard) => Update({...state, currentCard})
      | SetCollapsed(listId, isCollapsed) =>
        ReasonReact.UpdateWithSideEffects(
          {
            ...state,
            collapsedLists:
              isCollapsed ?
                state.collapsedLists |> Js.Array.concat([|listId|]) :
                state.collapsedLists |> Js.Array.filter(x => x !== listId),
          },
          (
            self =>
              Next.writeCookie(
                collapsedListsCookieName,
                self.state.collapsedLists |> Js.Array.joinWith("-"),
              )
          ),
        )
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
        |> TimeRecord.formatTodayRest
        |> Js.Array.map(
             fun
             | `Text(s) => ReasonReact.string(s)
             | `Time(s) => ReasonReact.string(s),
           )
        |> ReasonReact.array;

      let lists' =
        lists
        |> Js.Array.map((list: Trello.List.t) => {
             let isCollapsed =
               self.state.collapsedLists |> Js.Array.includes(list.id);

             let cards' =
               isCollapsed ?
                 ReasonReact.null :
                 cards
                 |> Js.Array.filter(card =>
                      card.Trello.Card.idList === list.id
                    )
                 |> Js.Array.map((card: Trello.Card.t) => {
                      let elementId = "current-card-radio__" ++ card.id;

                      let isCurrent =
                        switch (self.state.currentCard) {
                        | Some(x) => x === card
                        | None => false
                        };

                      let time =
                        Js.Dict.unsafeGet(self.state.records, card.id)
                        |> TimeRecord.formatTodayRest
                        |> Js.Array.map(
                             fun
                             | `Text(s) => ReasonReact.string(s)
                             | `Time(s) =>
                               isCurrent ?
                                 <Blink> (ReasonReact.string(s)) </Blink> :
                                 ReasonReact.string(s),
                           )
                        |> ReasonReact.array;

                      let onChange =
                        self.handle((_, self) =>
                          self.send(SetCurrentCard(Some(card)))
                        );

                      <li key=card.id>
                        <input
                          _type="radio"
                          name="current-card"
                          checked=isCurrent
                          onChange
                          id=elementId
                        />
                        <div>
                          <label htmlFor=elementId>
                            (card.name |> ReasonReact.string)
                          </label>
                          <p className=css##time> time </p>
                        </div>
                      </li>;
                    })
                 |> ReasonReact.array;

             let onClick =
               self.handle((_, self) =>
                 self.send(SetCollapsed(list.id, ! isCollapsed))
               );

             <div key=list.id className=css##listWrap>
               <h3>
                 (list.name |> ReasonReact.string)
                 <button className=css##toggleCollapse onClick>
                   (
                     ReasonReact.string(
                       isCollapsed ? "[expand]" : "[collapse]",
                     )
                   )
                 </button>
               </h3>
               cards'
             </div>;
           })
        |> ReasonReact.array;

      let onChange =
        self.handle((_, self) => self.send(SetCurrentCard(None)));

      let isResting = self.state.currentCard |> Js.Option.isNone;

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
              onChange
            />
            <div>
              <label htmlFor="current-card-radio__rest">
                ("Rest..." |> ReasonReact.string)
              </label>
            </div>
          </li>
        </ul>
        lists'
      </div>;
    },
  };
};

type data =
  | WithTrkrField(
      string,
      Trello.Board.t,
      Js.Array.t(Trello.List.t),
      array(Trello.Card.t),
      array(Js.String.t),
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
                |> flatMap(lists =>
                     Trello.fetchCards(boardId)
                     |> flatMap(cards =>
                          Trello.getCookies()
                          |> map(cookies => {
                               ignore(Js.Array.reverseInPlace(lists));

                               let initialCollapsedLists =
                                 switch (
                                   Js.Dict.get(
                                     cookies,
                                     collapsedListsCookieName,
                                   )
                                 ) {
                                 | Some(lists) => Js.String.split("-", lists)
                                 | None => [||]
                                 };

                               WithTrkrField(
                                 fieldId,
                                 board,
                                 lists,
                                 cards,
                                 initialCollapsedLists,
                               );
                             })
                        )
                   )
              }
            )
       )
    |> Trello.makeLoader
  );

let component = ReasonReact.statelessComponent("BoardsPage");

let default =
  Next.Page.create(~component, ~loader, props =>
    {
      ...component,
      render: _self =>
        ErrorPage.renderError(props, data =>
          switch (data) {
          | WithoutTrkrField(board) => renderUninteractive(board)
          | WithTrkrField(fieldId, board, lists, cards, initialCollapsedLists) =>
            <InteractiveBoard
              fieldId
              board
              lists
              cards
              initialCollapsedLists
            />
          }
        ),
    }
  );