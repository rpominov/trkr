import {ApiMonad} from "./api"
import TimeRecord from "./TimeRecord"

const UNTRACKED_TIME = "untrackedTime"

export function getTrkrFieldValue(card, fieldId) {
  const field = card.customFieldItems.filter(
    f => f.idCustomField === fieldId,
  )[0]
  return field ? field.value.text : ""
}

export function getTrkrRecord(card, fieldId) {
  return TimeRecord.parse(getTrkrFieldValue(card, fieldId))
}

export function getTrkrFieldId(board) {
  const trkrField = board.customFields.filter(f => f.name === "trkr")[0]
  return trkrField ? trkrField.id : null
}

export function getAllTrkrRecords(cards, fieldId) {
  const localData = getLocalData()
  const result = {}
  cards.forEach(card => {
    result[card.id] = TimeRecord.combineRecords([
      getTrkrRecord(card, fieldId),
      localData[card.id] || {},
    ])
  })
  return result
}

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

function getLocalData() {
  if (!process.browser) {
    return {}
  }

  const data = localStorage.getItem(UNTRACKED_TIME)
  return data ? JSON.parse(data) : {}
}

function setLocalData(data) {
  if (!process.browser) {
    return
  }

  localStorage.setItem(UNTRACKED_TIME, JSON.stringify(data))
}

function removeLocalDataForCard(cardId) {
  const localData = getLocalData()
  delete localData[cardId]
  setLocalData(localData)
}

export function getUntrackedTime() {
  const localData = getLocalData()
  return TimeRecord.aggregateTime(
    TimeRecord.combineRecords(Object.values(localData)),
  )
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
