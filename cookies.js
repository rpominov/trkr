import cookie from "cookie"

export function readCookie(getInitialPropsContext = {}) {
  const cookieString = getInitialPropsContext.req
    ? getInitialPropsContext.req.headers.cookie
    : document.cookie
  return cookie.parse(cookieString || "")
}

export function writeCookie(
  {name, value, maxAge},
  getInitialPropsContext = {},
) {
  const cookieString = cookie.serialize(name, value, {maxAge})
  if (getInitialPropsContext.res) {
    getInitialPropsContext.res.setHeader("Set-Cookie", cookieString)
  } else {
    document.cookie = cookieString
  }
}
