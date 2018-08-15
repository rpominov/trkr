let css: {
  .
  "layoutWrap": string,
  "title": string,
  "header": string,
} = [%raw
  {|require('components/Layout.css')|}
];

type isLoading = bool;

type action =
  | SetLoadingStart
  | SetLoadingStop;

let component = ReasonReact.reducerComponent("Layout");

let make = (children: array(ReasonReact.reactElement)) => {
  ...component,
  initialState: () => false,
  reducer: (action: action, _state: isLoading) =>
    switch (action) {
    | SetLoadingStart => Update(true)
    | SetLoadingStop => Update(false)
    },
  didMount: self => {
    let start = Some(() => self.send(SetLoadingStart));
    let stop = Some(() => self.send(SetLoadingStop));
    ReasonNext.Router.onRouteChangeStart(start);
    ReasonNext.Router.onRouteChangeComplete(stop);
    ReasonNext.Router.onRouteChangeError(stop);
    self.onUnmount(() => {
      ReasonNext.Router.onRouteChangeStart(None);
      ReasonNext.Router.onRouteChangeComplete(None);
      ReasonNext.Router.onRouteChangeError(None);
    });
  },
  render: self =>
    <div className=css##layoutWrap>
      <Favicon />
      <ReasonNext.Head>
        <title> {ReasonReact.string("TRKR")} </title>
      </ReasonNext.Head>
      <header className=css##header>
        <ReasonNext.Link href="/">
          <a>
            <h1 className=css##title>
              {
                self.state ?
                  <Distorted text="TRKR" /> : ReasonReact.string("TRKR")
              }
            </h1>
          </a>
        </ReasonNext.Link>
        <UntrackedTime />
      </header>
      <main> {ReasonReact.array(children)} </main>
    </div>,
};

let default =
  ReasonReact.wrapReasonForJs(~component, props => make(props##children));