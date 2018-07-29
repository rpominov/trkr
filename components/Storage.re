module Process = {
  [@bs.deriving abstract]
  type t = pri {browser: Js.Nullable.t(bool)};

  [@bs.val] external process : t = "";

  let inBrowser =
    switch (process |. browser |> Js.Nullable.toOption) {
    | Some(x) => x
    | None => false
    };
};

module LocalStorage = {
  type t;

  [@bs.send] external getItem : (t, string) => Js.Nullable.t(string) = "";
  [@bs.send] external setItem : (t, string, string) => unit = "";

  [@bs.val] external localStorage : t = "";
};

type t = Js.Dict.t(TimeRecord.t);

let localStorageKey = "untrackedTime";

let readLocalStorage = () : t =>
  if (! Process.inBrowser) {
    Js.Dict.empty();
  } else {
    switch (
      LocalStorage.(getItem(localStorage, localStorageKey))
      |> Js.Nullable.toOption
    ) {
    | Some(data) =>
      try (data |> Json.parseOrRaise |> Json.Decode.(int |> dict |> dict)) {
      | _ => Js.Dict.empty()
      }
    | None => Js.Dict.empty()
    };
  };

let writeLocalStorage = (data: t) : unit =>
  if (! Process.inBrowser) {
    ();
  } else {
    switch (Js.Json.stringifyAny(data)) {
    | Some(str) => LocalStorage.(setItem(localStorage, localStorageKey, str))
    | None => ()
    };
  };

let removeLocalRecord = (~cardId: string) =>
  readLocalStorage()
  |> Js.Dict.entries
  |> Js.Array.filter(((key, _)) => key !== cardId)
  |> Js.Dict.fromArray
  |> writeLocalStorage;

let getRecords = (cards: array(Trello.Card.t), ~fieldId: string) : t => {
  let localData = readLocalStorage();
  let result = Js.Dict.empty();

  cards
  |> Js.Array.forEach(card => {
       let {Trello.Card.id} = card;
       TimeRecord.(
         switch (
           Trello.Card.getCustomField(~fieldId, card),
           Js.Dict.get(localData, id),
         ) {
         | (Some(a), Some(b)) => combineRecords([|parse(a), b|])
         | (Some(a), None) => parse(a)
         | (None, Some(b)) => b
         | (None, None) => empty()
         }
       )
       |> Js.Dict.set(result, id);
     });

  result;
};

let totalUntracked = () =>
  readLocalStorage()
  |> Js.Dict.values
  |> TimeRecord.combineRecords
  |> TimeRecord.aggregateTime;

let track' =
    (~cardId: string, ~append: TimeRecord.t, ~fieldId: option(string)=?, ()) => {
  let tmp = 1;
  ();
};

/*

 async function track_(cardId, recordToAppend, trkrFieldId = null) {
   return ApiMonad.run(
     ApiMonad.do(function*() {
       const card = yield ApiMonad.call([
         `cards/${cardId}`,
         {
           customFieldItems: "true",
           fields: "name,idBoard",
         },
       ])

       if (trkrFieldId === null) {
         const board = yield ApiMonad.call([
           `boards/${card.idBoard}`,
           {fields: "name", customFields: "true"},
         ])
         trkrFieldId = getTrkrFieldId(board)
       }

       const newValue = TimeRecord.stringify(
         TimeRecord.combineRecords([
           getTrkrRecord(card, trkrFieldId),
           recordToAppend,
         ]),
       )

       yield ApiMonad.call([
         `card/${cardId}/customField/${trkrFieldId}/item`,
         {},
         "PUT",
         {
           value: {text: newValue},
         },
       ])

       return newValue
     }),
   )
 }

 export async function track(cardId, timeAmmount, trkrFieldId) {
   const recordToAppend = TimeRecord.increment({}, timeAmmount)
   const result = await track_(cardId, recordToAppend, trkrFieldId)

   if (result.tag === "error") {
     const localData = getLocalData()
     const localCardData = localData[cardId] || {}
     const newLocalCardData = TimeRecord.combineRecords([
       localCardData,
       recordToAppend,
     ])
     setLocalData({...localData, [cardId]: newLocalCardData})
   }

   return recordToAppend
 }

 export async function reSend() {
   const localData = getLocalData()
   const cardIds = Object.keys(localData)

   for (const id of cardIds) {
     const recordToAppend = localData[id]
     const result = await track_(id, recordToAppend)
     if (result.tag === "error") {
       return result
     }
     removeLocalDataForCard(id)
   }

   return null
 }

 */