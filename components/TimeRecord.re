type t = Js.Dict.t(int);

type key = string;

let separator1 = ", ";
let separator2 = " ";
let separator3 = ":";
let separator4 = ".";

let empty = () : t => Js.Dict.empty();

let getCurrentDate = () : key => {
  open Js.Date;
  let d = make();
  [|
    d |> getDate |> Js.String.make,
    d |> getMonth |> truncate |> (x => x + 1) |> Js.String.make,
    d |> getFullYear |> Js.String.make |> Js.String.substr(~from=2),
  |]
  |> Js.Array.joinWith(separator4);
};

let decomposeTime = seconds => {
  let h = seconds / 60 / 60;
  let m = (seconds - h * 60 * 60) / 60;
  let s = seconds - h * 60 * 60 - m * 60;
  (h, m, s);
};

let stringifyTime = seconds => {
  let (h, m, s) = decomposeTime(seconds);
  Js.Array.joinWith(separator3, [|h, m, s|]);
};

let getTodayTime = record =>
  switch (Js.Dict.get(record, getCurrentDate())) {
  | None => 0
  | Some(time) => time
  };

let aggregateTime = record =>
  Js.Array.reduce((+), 0, Js.Dict.values(record));

let combineAll = (records: array(t)) : t => {
  let result = empty();

  let insert = ((key, value)) =>
    (
      switch (Js.Dict.get(result, key)) {
      | None => value
      | Some(x) => value + x
      }
    )
    |> Js.Dict.set(result, key);

  Js.Array.forEach(record => Util.dictForEach(insert, record), records);

  result;
};

let combine = (extra: t, current: option(t)) : t =>
  switch (current) {
  | None => extra
  | Some(current') => combineAll([|extra, current'|])
  };

let increment = (record, by) => {
  let key = getCurrentDate();
  (
    switch (Js.Dict.get(record, key)) {
    | None => by
    | Some(x) => by + x
    }
  )
  |> Util.dictAssoc(record, key);
};

let formatTime = seconds => {
  let tmp = float_of_int(seconds) *. 1000.0 /. 60.0 /. 60.0;
  tmp > 0.0 && tmp < 1.0 ?
    "0.001" : Js.String.make(Js.Math.round(tmp) /. 1000.0);
};

let stringify = record =>
  record
  |> Js.Dict.entries
  |> Js.Array.map(((key, value)) =>
       key ++ separator2 ++ stringifyTime(value)
     )
  |> Js.Array.joinWith(separator1);

/* We should use `separator...` constants in the regex */
let chunkRe = [%bs.re {|/^(\d\d?\.\d\d?\.\d\d)\s(\d\d?):(\d\d?):(\d\d?)$/|}];

let parse = str => {
  let result = empty();
  let i = int_of_string;

  str
  |> Js.String.split(separator1)
  |> Js.Array.forEach(chunk =>
       switch (Util.reExec(chunkRe, chunk)) {
       | [|_, Some(d), Some(h), Some(m), Some(s)|] =>
         Js.Dict.set(result, d, i(s) + i(m) * 60 + i(h) * 60 * 60)
       | _ => ()
       }
     );

  result;
};

let formatTodayRest = (~blink=false, record) => {
  let today = record |> getTodayTime |> formatTime;
  let all = record |> aggregateTime |> formatTime;

  let text = ReasonReact.string;
  let time =
    blink ?
      x => <Blink> (ReasonReact.string(x)) </Blink> : ReasonReact.string;

  let a = [|time(all), text(all == "1" ? " hour" : " hours")|];
  let b =
    today == "0" ? [||] : [|text(" ("), time(today), text(" today)")|];

  Js.Array.concat(b, a) |> ReasonReact.array;
};

let formatToday = record => record |> getTodayTime |> formatTime;