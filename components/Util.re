let dictAssoc = (dict, key, value) => {
  /* inefficient!!! */
  let copy = Js.Dict.fromArray(Js.Dict.entries(dict));
  Js.Dict.set(copy, key, value);
  copy;
};

let dictForEach = (f, dict) =>
  /* inefficient!!! */
  Js.Array.forEach(f, Js.Dict.entries(dict));

let reExec = (re, str) =>
  switch (Js.Re.exec(str, re)) {
  | Some(x) => x |> Js.Re.captures |> Js.Array.map(Js.Nullable.toOption)
  | None => [||]
  };

let renderError = (props, regularRender) => {
  let data = props |. Next.Page.Props.data;

  switch (data) {
  | Belt.Result.Ok(data') => regularRender(data')
  | Belt.Result.Error(error) => <ErrorPage error />
  };
};