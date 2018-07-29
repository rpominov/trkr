import React from "react"
import TimeRecord from "./TimeRecord"
import Distorted from "./components/react/Distorted.bs"
import {getUntrackedTime, reSend} from "./timeStorage"

export default class UntrackedTime extends React.Component {
  state = {
    time: 0,
    sending: false,
  }

  componentDidMount() {
    this.intervalId = setInterval(this.check, 600)
  }

  componentWillUnmount() {
    clearTimeout(this.intervalId)
  }

  check = () => {
    this.setState({time: getUntrackedTime()})
  }

  handleClick = () => {
    this.setState({sending: true})
    reSend().then(() => {
      this.setState({sending: false})
      this.check()
    })
  }

  render() {
    const {time, sending} = this.state

    if (time === 0) {
      return null
    }

    return (
      <div>
        <style jsx>{`
          div {
            padding: 14px;
            margin-top: 10px;
            background: #ffdfa7;
          }

          p {
            margin-bottom: 10px;
          }

          button {
            border: none;
            background: white;
            padding: 6px 12px;
            cursor: pointer;
          }

          button[disabled] {
            cursor: default;
          }
        `}</style>
        <p>
          We were unable to send {TimeRecord.formatTime(time)} hours of tracked
          time to the server.
        </p>
        <button onClick={this.handleClick} disabled={sending}>
          {sending ? <Distorted text="Try again now" /> : "Try again now"}
        </button>
      </div>
    )
  }
}
