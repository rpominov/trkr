module P = Js.Promise;
module R = Belt.Result;

type t('ok, 'error) = P.t(R.t('ok, 'error));

let ok = (x: 'a) : t('a, 'b) => R.Ok(x) |> P.resolve;

let error = (x: 'a) : t('b, 'a) => R.Error(x) |> P.resolve;

let wrap = (f: P.error => 'a, promise: P.t('b)) : t('b, 'a) =>
  promise
  |> P.then_(x => R.Ok(x) |> P.resolve)
  |> P.catch(e => R.Error(e |> f) |> P.resolve);

let map = (f: 'a => 'b, promise: t('a, 'c)) : t('b, 'c) =>
  promise
  |> P.then_(
       fun
       | R.Ok(x') => R.Ok(f(x')) |> P.resolve
       | R.Error(_) as e => e |> P.resolve,
     );

let flatMap = (f: 'a => t('b, 'c), promise: t('a, 'c)) : t('b, 'c) =>
  promise
  |> P.then_(
       fun
       | R.Ok(x') => f(x')
       | R.Error(_) as e => e |> P.resolve,
     );

let flatMapError = (f: 'b => t('a, 'b), promise: t('a, 'b)) : t('a, 'b) =>
  promise
  |> P.then_(
       fun
       | R.Ok(_) as ok => ok |> P.resolve
       | R.Error(e) => f(e),
     );

let recover = f => flatMapError(err => err |> f |> ok);

let tryMap = (mapOk, mapExn, promise) => {
  let mapper = x =>
    P.resolve(
      try (R.Ok(mapOk(x))) {
      | e => R.Error(mapExn(e, x))
      },
    );
  flatMap(mapper, promise);
};

let iterate = (f, items) => {
  let rec step = (i, acc) =>
    if (i >= Js.Array.length(items)) {
      ok(acc);
    } else {
      f(items[i]) |> flatMap(x => step(i + 1, Js.Array.concat(acc, [|x|])));
    };
  step(0, [||]);
};

let consume = (f: 'a => unit, promise) => promise |> map(f) |. ignore;

let consumeResult = (f: 'a => unit, promise) =>
  P.then_(x => f(x) |> P.resolve, promise) |. ignore;