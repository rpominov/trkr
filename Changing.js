import React from "react"

export default class Changing extends React.Component {
  componentDidMount() {
    this.intervalId = setInterval(() => {
      this.forceUpdate()
    }, this.props.period)
  }

  componentWillUnmount() {
    clearInterval(this.intervalId)
  }

  render() {
    const {text, skip} = this.props

    if (skip && Math.random() < skip) {
      return text
    }

    const symbols = ["#", "%", "_", "/", "5", "*"]
    let index
    do {
      index = Math.floor(Math.random() * text.length)
    } while (text[index] === " ")
    return (
      <>
        {text.substr(0, index) +
          symbols[Math.floor(Math.random() * symbols.length)] +
          text.substr(index + 1)}
      </>
    )
  }
}
