import React from "react"
import Link from "next/link"
import Layout from "../Layout"
import Error from "../Error"
import api from "../api"
import {readCookie} from "../cookies"
import {autoAuthenticate} from "../redirect"

const Board = p => {
  const {board} = p
  const color = board.prefs.backgroundBrightness === "dark" ? "white" : "black"

  const backgroundImage = board.prefs.backgroundImage
    ? `url(${board.prefs.backgroundImageScaled[3].url})`
    : null

  const backgroundColor = board.prefs.backgroundImage
    ? "#666"
    : board.prefs.background

  return (
    <li>
      <style jsx>{`
        li {
          display: inline-block;
          width: 300px;
          margin: 0 20px 20px 0;
        }

        a {
          padding: 90px 10px 10px 10px;
          display: block;
          background-size: cover;
          background-position: center;
        }
      `}</style>
      <Link href={{pathname: "/board", query: {id: board.id}}}>
        <a style={{color, backgroundImage, backgroundColor}}>{board.name}</a>
      </Link>
    </li>
  )
}

export default class extends React.Component {
  static async getInitialProps(context) {
    const {username} = readCookie(context)

    const resp = await api(context, `members/me/boards`, {
      filter: "starred",
      fields: "name,id,prefs",
    })

    if (resp.tag === "success") {
      return {boards: resp.result}
    }

    if (autoAuthenticate(resp, context)) {
      return {}
    }

    return {boards: null, error: resp}
  }

  render() {
    const {boards, error} = this.props

    if (boards) {
      return (
        <Layout>
          <ul>{boards.map(b => <Board board={b} key={b.id} />)}</ul>
        </Layout>
      )
    }

    return <Error error={error} />
  }
}
