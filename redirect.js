import Router from "next/router"
import getBaseUrl from "./getBaseUrl"

const {APP_KEY} = process.env

export function redirect(target, getInitialPropsContext = {}) {
  if (getInitialPropsContext.res) {
    getInitialPropsContext.res.writeHead(303, {Location: target})
    getInitialPropsContext.res.end()
  } else {
    Router.replace(target)
  }
}

export function autoAuthenticate(error, getInitialPropsContext = {}) {
  if (error.kind === "unauthenticated") {
    const url =
      "https://trello.com/1/authorize?" +
      [
        `name=${encodeURIComponent("TRKR Time Tracker")}`,
        "scope=read,write",
        `key=${APP_KEY}`,
        `return_url=${encodeURIComponent(
          getBaseUrl(getInitialPropsContext) + "/login",
        )}`,
      ].join("&")
    redirect(url, getInitialPropsContext)
    return true
  }
  return false
}
