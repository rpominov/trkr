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
    if (seconds === 0) {
      return "0s"
    }

    const {h, m, s} = TimeRecord.decomposeTime(seconds)

    return [h > 0 && h + "h", (m > 0 || h > 0) && m + "m", s > 0 && s + "s"]
      .filter(Boolean)
      .join(" ")
  },

  getTodayTime(record) {
    return record[TimeRecord.getCurrentDate()] || 0
  },

  aggregateTime(record) {
    return Object.keys(record).reduce((sum, key) => sum + record[key], 0)
  },

  formatTodayRest(text) {
    const record = TimeRecord.parse(text)
    const today = TimeRecord.getTodayTime(record)
    const all = TimeRecord.aggregateTime(record)
    return (
      TimeRecord.formatTime(all) +
      (today > 0 ? ` (${TimeRecord.formatTime(today)} today)` : "")
    )
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
