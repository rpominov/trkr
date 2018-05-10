import fetch from "isomorphic-unfetch"
import config from "./config"

export default async function api(path, _params, method = "GET", data = null) {
  const params = Object.keys(_params)
    .map(name => `${name}=${_params[name]}`)
    .join("&")

  let text

  try {
    const resp = await fetch(
      `https://api.trello.com/1/${path}?${params}&key=${config.key}&token=${
        config.token
      }`,
      {
        method,
        cache: "no-cache",
        headers: data ? {"content-type": "application/json"} : undefined,
        body: data ? JSON.stringify(data) : undefined,
      },
    )
    text = await resp.text()
  } catch (e) {
    return {tag: "error", kind: "network", message: e.message}
  }

  try {
    return {tag: "success", result: JSON.parse(text)}
  } catch (e) {
    return {tag: "error", kind: "api", message: text}
  }
}
