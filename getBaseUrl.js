export default function getBaseUrl(getInitialPropsContext) {
  const {req} = getInitialPropsContext

  if (req) {
    return (
      (req.connection.encrypted ? "https" : "http") + "://" + req.headers.host
    )
  }

  return location.protocol + "//" + location.host
}
