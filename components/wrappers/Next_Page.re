module Props = {
  [@bs.deriving abstract]
  type t('a) = {data: 'a};
};

module InnerProps = {
  [@bs.deriving abstract]
  type t('a) = {data: 'a};
};

let decodeProps = (props: Props.t(SafePass.t)) : Props.t('a) =>
  Props.t(~data=SafePass.decode(props |. Props.data));

let create =
    (
      ~loader: Next_LoadingContext.t => Js.Promise.t('a),
      ~component,
      converter:
        Props.t('a) => ReasonReact.component('state, 'retainedProps, 'action),
    ) => {
  let reactClass =
    ReasonReact.wrapReasonForJs(~component, props =>
      props |> decodeProps |> converter
    );

  let loader' = ctx =>
    Js.Promise.(
      loader(ctx)
      |> then_(x => InnerProps.t(~data=SafePass.encode(x)) |> resolve)
    );

  Obj.magic(reactClass)##getInitialProps#=loader';
  reactClass;
};