module Props = {
  [@bs.deriving abstract]
  type t('a) = {
    data: 'a,
    router: Next_Router.t,
  };
};

module InnerProps = {
  [@bs.deriving abstract]
  type t('a) = {data: 'a};
};

let decodeProps = (props: Props.t(PreserveTags.t('a))): Props.t('a) =>
  Props.t(
    ~data=PreserveTags.decode(props->Props.dataGet),
    ~router=props->Props.routerGet,
  );

let create =
    (
      ~loader: Next_LoadingContext.t => Js.Promise.t('a),
      ~component:
         ReasonReact.componentSpec(
           'state,
           'initialState,
           'retainedProps,
           'initialRetainedProps,
           'action,
         ),
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
      |> then_(x => InnerProps.t(~data=PreserveTags.encode(x)) |> resolve)
    );

  Obj.magic(reactClass)##getInitialProps #= loader';
  reactClass;
};
