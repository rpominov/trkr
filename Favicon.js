import React from "react"
import Head from "next/head"

const frames = [
  "/static/favicon/icon-1.png",
  "/static/favicon/icon-2.png",
  "/static/favicon/icon-3.png",
  "/static/favicon/icon-4.png",
  "/static/favicon/icon-5.png",
  "/static/favicon/icon-6.png",
  "/static/favicon/icon-7.png",
  "/static/favicon/icon-8.png",
  "/static/favicon/icon-9.png",
  "/static/favicon/icon-10.png",
  "/static/favicon/icon-11.png",
  "/static/favicon/icon-12.png",
]

export default class Layout extends React.Component {
  state = {
    currentFrame: 0,
  }

  componentDidMount() {
    if (this.props.dynamic) {
      this.intervalId = setInterval(() => {
        const {currentFrame} = this.state
        this.setState({currentFrame: (currentFrame + 1) % frames.length})
      }, 240)
    }
  }

  componentWillUnmount() {
    if (this.intervalId !== undefined) {
      clearTimeout(this.intervalId)
    }
  }

  render() {
    const {currentFrame} = this.state
    return (
      <Head>
        <link rel="icon" type="image/png" href={frames[currentFrame]} />
      </Head>
    )
  }
}
