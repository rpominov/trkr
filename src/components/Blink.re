let css: {. "main": string} = [%raw {|require('components/Blink.css')|}];

let component = ReasonReact.statelessComponent("Blink");

let make = children => {
  ...component,
  render: _self =>
    <span className=css##main> (ReasonReact.array(children)) </span>,
};