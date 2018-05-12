import fetch from "isomorphic-unfetch"
import getConfig from "next/config"
import {readCookie} from "./cookies"
import {autoAuthenticate} from "./redirect"

const {appKey} = getConfig().publicRuntimeConfig

export default async function api(
  getInitialPropsContext,
  path,
  params,
  method = "GET",
  data = null,
) {
  const {token} = readCookie(getInitialPropsContext || {})
  const args = {path, params, method, data}

  if (!token) {
    return {tag: "error", kind: "unauthenticated"}
  }

  const params_ = Object.keys(params)
    .map(name => `${name}=${params[name]}`)
    .join("&")

  const body = data ? JSON.stringify(data) : undefined
  const headers = body ? {"content-type": "application/json"} : undefined

  let resp
  try {
    resp = await fetch(
      `https://api.trello.com/1/${path}?${params_}&key=${appKey}&token=${token}`,
      {method, cache: "no-cache", headers, body},
    )
  } catch (e) {
    return {tag: "error", kind: "network", message: e.message, args}
  }

  if (resp.status === 401) {
    return {tag: "error", kind: "unauthenticated", args}
  }

  if (resp.status !== 200) {
    return {tag: "error", kind: "http-status-" + resp.status, args}
  }

  const text = await resp.text()

  try {
    return {tag: "success", result: JSON.parse(text)}
  } catch (e) {
    return {tag: "error", kind: "api", message: text, args}
  }
}

export const ApiMonad = {
  of(x) {
    return {tag: "of", x}
  },

  error(e) {
    return {tag: "error", e}
  },

  call(params) {
    return {tag: "call", params}
  },

  chain(fn, parent) {
    return {tag: "chain", fn, parent}
  },

  do(generator) {
    const gen = generator()
    function recur(result) {
      return result.done
        ? ApiMonad.of(result.value)
        : ApiMonad.chain(x => recur(gen.next(x)), result.value)
    }
    return recur(gen.next())
  },

  async run(item, getInitialPropsContext) {
    if (item.tag === "of") {
      return {tag: "success", result: item.x}
    }
    if (item.tag === "error") {
      return item.e
    }
    if (item.tag === "call") {
      return api(getInitialPropsContext, ...item.params)
    }
    if (item.tag === "chain") {
      const parentResult = await ApiMonad.run(
        item.parent,
        getInitialPropsContext,
      )
      if (parentResult.tag === "success") {
        return await ApiMonad.run(
          item.fn(parentResult.result),
          getInitialPropsContext,
        )
      } else {
        return parentResult
      }
    }
  },

  async runAndHandleError(item, getInitialPropsContext) {
    const result = await ApiMonad.run(item, getInitialPropsContext)
    if (result.tag === "error") {
      autoAuthenticate(result, getInitialPropsContext)
      return {error: result}
    }
    return result.result
  },

  async doAndHandleError(getInitialPropsContext, generator) {
    return ApiMonad.runAndHandleError(
      ApiMonad.do(generator),
      getInitialPropsContext,
    )
  },
}

// export async fucntion sequnce(getInitialPropsContext, items) {
//   async function runSingle(item, input) {
//     if (typeof item === 'fucntion') {
//       return runSingle(item(input))
//     }
//     if (item.action === 'end') {
//       return item.data
//     }
//     if (item.action === 'error') {

//     }

//     return api(getInitialPropsContext, )
//   }
// }
