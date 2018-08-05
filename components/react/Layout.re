let css: {
  .
  "layoutWrap": string,
  "title": string,
  "header": string,
} = [%raw
  {|require('./Layout.css')|}
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
    open Next.Router;
    let start = Js.Nullable.return(() => self.send(SetLoadingStart));
    let stop = Js.Nullable.return(() => self.send(SetLoadingStop));
    onRouteChangeStartSet(router, start);
    onRouteChangeCompleteSet(router, stop);
    onRouteChangeErrorSet(router, stop);
    self.onUnmount(() => {
      onRouteChangeStartSet(router, Js.Nullable.null);
      onRouteChangeCompleteSet(router, Js.Nullable.null);
      onRouteChangeErrorSet(router, Js.Nullable.null);
    });
  },
  render: self =>
    <div className=css##layoutWrap>
      <Favicon />
      <Next.Head> <title> (ReasonReact.string("TRKR")) </title> </Next.Head>
      <header className=css##header>
        <Next.Link href="/">
          <a>
            <h1 className=css##title>
              (
                self.state ?
                  <Distorted text="TRKR" /> : ReasonReact.string("TRKR")
              )
            </h1>
          </a>
        </Next.Link>
        <UntrackedTime />
      </header>
      <main> (ReasonReact.array(children)) </main>
    </div>,
};

let default =
  ReasonReact.wrapReasonForJs(~component, props => make(props##children));