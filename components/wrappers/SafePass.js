const tagRep = "tag7551544577058851"
const undefinedRep = "undefined6940448865807247"

function encodeArray(arr) {
  return arr.map(encode)
}

function encodeDict(dict) {
  let result = {}
  for (key in dict) {
    result[key] = encode(dict[key])
  }
  return result
}

function encode(data) {
  if (Array.isArray(data)) {
    return data.tag !== undefined
      ? {[tagRep]: data.tag, array: encodeArray(data)}
      : encodeArray(data)
  }

  if (typeof data === "object" && data !== null) {
    return encodeDict(data)
  }

  if (data === undefined) {
    return undefinedRep
  }

  return data
}

function decodeArray(arr) {
  return arr.map(decode)
}

function decodeDict(dict) {
  const result = {}
  for (key in dict) {
    result[key] = decode(dict[key])
  }
  return result
}

function decode(data) {
  if (typeof data === "object" && data !== null) {
    if (data[tagRep] !== undefined) {
      const result = decodeArray(data.array)
      result.tag = data[tagRep]
      return result
    } else {
      return Array.isArray(data) ? decodeArray(data) : decodeDict(data)
    }
  }

  if (data === undefinedRep) {
    return undefined
  }

  return data
}

exports.encode = encode
exports.decode = decode
