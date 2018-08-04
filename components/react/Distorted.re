let symbols = [|"#", "%", "_", "/", "5", "*"|];

let distort = text => {
  let i = Js.Math.random_int(0, Js.String.length(text));
  let j = Js.Math.random_int(0, Js.Array.length(symbols));
  Js.String.(
    substring(~from=0, ~to_=i, text)
    ++ symbols[j]
    ++ substr(~from=i + 1, text)
  );
};

let component = ReasonReact.reducerComponent("Distorted");

let make = (~text, ~period=100, _children) => {
  ...component,
  reducer: (_action, state) => ReasonReact.Update(state),
  didMount: ({send, onUnmount}) => {
    let id = Js.Global.setInterval(send, period);
    onUnmount(() => Js.Global.clearInterval(id));
  },
  render: _self => ReasonReact.string(distort(text)),
};

/* TMP JS interop */

[@bs.deriving abstract]
type jsProps = {
  text: string,
  period: Js.nullable(int),
};

let default =
  ReasonReact.wrapReasonForJs(~component, props =>
    make(
      ~text=text(props),
      ~period=?Js.Nullable.toOption(period(props)),
      [||],
    )
  );