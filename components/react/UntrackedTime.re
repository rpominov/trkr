let css: {. "main": string} = [%raw {|require('./UntrackedTime.css')|}];

type state = {
  time: int,
  sending: bool,
};

type action =
  | CheckTime
  | SendStart
  | SendStop;

let component = ReasonReact.reducerComponent("UntrackedTime");

let make = _children => {
  ...component,
  initialState: () => {time: 0, sending: false},
  reducer: (action: action, state: state) =>
    switch (action) {
    | CheckTime => Update({...state, time: Storage.totalUntracked()})
    | SendStart =>
      state.sending ?
        NoUpdate :
        UpdateWithSideEffects(
          {...state, sending: true},
          (
            self =>
              Storage.reSend()
              |> PromiseResult.consumeResult(_ => self.send(SendStop))
          ),
        )
    | SendStop => Update({time: Storage.totalUntracked(), sending: false})
    },
  didMount: self => {
    let id = Js.Global.setInterval(() => self.send(CheckTime), 600);
    self.onUnmount(() => Js.Global.clearInterval(id));
  },
  render: self =>
    self.state.time === 0 ?
      ReasonReact.null :
      <div className=css##main>
        <p>
          ("We were unable to send " |. ReasonReact.string)
          (self.state.time |. TimeRecord.formatTime |. ReasonReact.string)
          (" hours of tracked time to the server." |. ReasonReact.string)
        </p>
        <button
          onClick=(_ => self.send(SendStart)) disabled=self.state.sending>
          (
            self.state.sending ?
              <Distorted text="Try again now" /> :
              ReasonReact.string("Try again now")
          )
        </button>
      </div>,
};