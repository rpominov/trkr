import React from "react"
import {writeCookie} from "../cookies"
import {redirect} from "../redirect"

export default class extends React.Component {
  componentDidMount() {
    const token = this.props.url.asPath.split("#")[1].split("=")[1]
    writeCookie({name: "token", value: token, maxAge: 60 * 60 * 24 * 356})
    redirect("/")
  }

  render() {
    return <div>redirecting...</div>
  }
}
