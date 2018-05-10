import fetch from "isomorphic-unfetch"
import getConfig from "next/config"
import {readCookie} from "./cookies"

const {appKey} = getConfig().publicRuntimeConfig

export default async function api(
  getInitialPropsContext,
  path,
  _params,
  method = "GET",
  data = null,
) {
  const {token} = readCookie(getInitialPropsContext || {})

  if (!token) {
    return {tag: "error", kind: "unauthenticated"}
  }

  const params = Object.keys(_params)
    .map(name => `${name}=${_params[name]}`)
    .join("&")

  const body = data ? JSON.stringify(data) : undefined
  const headers = body ? {"content-type": "application/json"} : undefined

  let resp
  try {
    resp = await fetch(
      `https://api.trello.com/1/${path}?${params}&key=${appKey}&token=${token}`,
      {method, cache: "no-cache", headers, body},
    )
  } catch (e) {
    return {tag: "error", kind: "network", message: e.message}
  }

  if (resp.status === 401) {
    return {tag: "error", kind: "unauthenticated"}
  }

  if (resp.status !== 200) {
    return {tag: "error", kind: "http-status-" + resp.status}
  }

  const text = await resp.text()

  try {
    return {tag: "success", result: JSON.parse(text)}
  } catch (e) {
    return {tag: "error", kind: "api", message: text}
  }
}
