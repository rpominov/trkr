let css: {. "main": string} = [%raw {|require('components/Blink.css')|}];

let render = child => <span className=css##main> child </span>;
