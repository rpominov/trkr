import Router from "next/router"

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
        `return_url=${encodeURIComponent("http://localhost:3000/login")}`,
        `key=${APP_KEY}`,
      ].join("&")
    redirect(url, getInitialPropsContext)
    return true
  }
  return false
}
