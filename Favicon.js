import React from "react"
import Head from "next/head"

const staticIcon = require("./static/favicon/icon-0.png")

const frames = [
  require("./static/favicon/icon-1.png"),
  require("./static/favicon/icon-2.png"),
  require("./static/favicon/icon-3.png"),
  require("./static/favicon/icon-4.png"),
  require("./static/favicon/icon-5.png"),
  require("./static/favicon/icon-6.png"),
  require("./static/favicon/icon-7.png"),
  require("./static/favicon/icon-8.png"),
  require("./static/favicon/icon-9.png"),
  require("./static/favicon/icon-10.png"),
  require("./static/favicon/icon-11.png"),
  require("./static/favicon/icon-12.png"),
]

export default class Layout extends React.Component {
  state = {
    currentFrame: 0,
  }

  componentDidMount() {
    this.intervalId = setInterval(() => {
      if (this.props.animated) {
        const {currentFrame} = this.state
        this.setState({currentFrame: (currentFrame + 1) % frames.length})
      }
    }, 240)
  }

  componentWillUnmount() {
    clearTimeout(this.intervalId)
  }

  render() {
    const {animated} = this.props
    const {currentFrame} = this.state

    const image = animated ? frames[currentFrame] : staticIcon

    return (
      <Head>
        <link rel="icon" type="image/png" href={image} />
      </Head>
    )
  }
}
