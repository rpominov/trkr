let staticIcon: string = [%raw {|require('static/favicon/icon-0.png')|}];

let frames: array(string) = [%raw
  {|[
  require("static/favicon/icon-1.png"),
  require("static/favicon/icon-2.png"),
  require("static/favicon/icon-3.png"),
  require("static/favicon/icon-4.png"),
  require("static/favicon/icon-5.png"),
  require("static/favicon/icon-6.png"),
  require("static/favicon/icon-7.png"),
  require("static/favicon/icon-8.png"),
  require("static/favicon/icon-9.png"),
  require("static/favicon/icon-10.png"),
  require("static/favicon/icon-11.png"),
  require("static/favicon/icon-12.png"),
]|}
];

type currentFrame = int;

type action =
  | Increment;

let component = ReasonReact.reducerComponent("Distorted");

let make = (~animated=false, _children) => {
  ...component,
  initialState: () => 0,
  reducer: (_action: action, state: currentFrame) =>
    ReasonReact.Update((state + 1) mod Js.Array.length(frames)),
  didMount: ({send, onUnmount}) => {
    let id = Js.Global.setInterval(() => Increment |. send, 240);
    onUnmount(() => Js.Global.clearInterval(id));
  },
  render: self => {
    let image = animated ? frames[self.state] : staticIcon;
    <Next.Head> <link rel="icon" _type="image/png" href=image /> </Next.Head>;
  },
};