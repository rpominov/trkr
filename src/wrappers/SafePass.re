/* https://github.com/BuckleScript/bucklescript/issues/2952 */

%bs.raw
{|
const tagRep = "tag7551544577058851"
const undefinedRep = "undefined6940448865807247"

function _encode(data) {
  if (Array.isArray(data)) {
    return data.tag !== undefined
      ? {[tagRep]: data.tag, array: data.map(encode)}
      : data.map(encode)
  }

  if (typeof data === "object" && data !== null) {
    const result = {}
    for (key in data) {
      result[key] = encode(data[key])
    }
    return result
  }

  if (data === undefined) {
    return undefinedRep
  }

  return data
}

function _decode(data) {
  if (typeof data === "object" && data !== null) {
    if (data[tagRep] !== undefined) {
      const result = data.array.map(decode)
      result.tag = data[tagRep]
      return result
    } else {
      if (Array.isArray(data)) {
        return data.map(decode)
      } else {
        const result = {}
        for (key in data) {
          result[key] = decode(data[key])
        }
        return result
      }
    }
  }

  if (data === undefinedRep) {
    return undefined
  }

  return data
}
|};

type t('a);

[@bs.val] external _encode : 'a => t('a) = "";
[@bs.val] external _decode : t('a) => 'a = "";

let encode = _encode;
let decode = _decode;