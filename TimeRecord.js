const TimeRecord = {
  chunkSeparator: ", ",

  getCurrentDate() {
    const d = new Date()
    return [
      d.getDate(),
      d.getMonth() + 1,
      String(d.getFullYear()).substr(2),
    ].join(".")
  },

  isValidChunk(str) {
    return /^\d\d?\.\d\d?\.\d\d\s\d\d?:\d\d?:\d\d?$/.test(str)
  },

  parseTime(str) {
    const [h, m, s] = str.split(":")
    return Number(s) + Number(m) * 60 + Number(h) * 60 * 60
  },

  decomposeTime(seconds) {
    const h = Math.floor(seconds / 60 / 60)
    const m = Math.floor((seconds - h * 60 * 60) / 60)
    const s = seconds - h * 60 * 60 - m * 60
    return {h, m, s}
  },

  stringifyTime(seconds) {
    const {h, m, s} = TimeRecord.decomposeTime(seconds)
    return [h, m, s].join(":")
  },

  formatTime(seconds) {
    const tmp = seconds * 100 / 60 / 60
    if (tmp > 0 && tmp < 1) {
      return 0.01
    }
    return Math.round(tmp) / 100
  },

  getTodayTime(record) {
    return record[TimeRecord.getCurrentDate()] || 0
  },

  aggregateTime(record) {
    return Object.keys(record).reduce((sum, key) => sum + record[key], 0)
  },

  combineRecords(records) {
    const result = {}
    records.forEach(record => {
      Object.keys(record).forEach(key => {
        result[key] = (result[key] || 0) + record[key]
      })
    })
    return result
  },

  formatTodayRest(record) {
    const today = TimeRecord.formatTime(TimeRecord.getTodayTime(record))
    const allFormated = TimeRecord.formatTime(TimeRecord.aggregateTime(record))
    return [allFormated, allFormated === 1 ? " hour" : " hours"].concat(
      today > 0 ? [" (", today, " today)"] : [],
    )
  },

  formatToday(text) {
    const record = TimeRecord.parse(text)
    const today = TimeRecord.getTodayTime(record)
    return TimeRecord.formatTime(today)
  },

  parse(text) {
    const chunks = text
      .split(TimeRecord.chunkSeparator)
      .filter(TimeRecord.isValidChunk)

    const result = {}

    chunks.forEach(chunk => {
      const [date, time] = chunk.split(" ")
      result[date] = TimeRecord.parseTime(time)
    })

    return result
  },

  stringify(record) {
    const asArray = Object.keys(record).map(k => [k, record[k]])
    // asArray.sort(TODO)
    return asArray
      .map(x => x[0] + " " + TimeRecord.stringifyTime(x[1]))
      .join(TimeRecord.chunkSeparator)
  },

  increment(record, by) {
    const date = TimeRecord.getCurrentDate()
    const currentTime = record[date] || 0
    return {...record, [date]: currentTime + by}
  },
}

export default TimeRecord
