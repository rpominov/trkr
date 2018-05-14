import React from "react"
import {getUntrackedTime, reSend} from "./localTimeStorage"

export default class UntrackedTime extends React.Component {
  state = {
    time: 0,
  }

  componentDidMount() {
    this.intervalId = setInterval(() => {
      this.setState({time: getUntrackedTime()})
    }, 600)
  }

  componentWillUnmount() {
    clearTimeout(this.intervalId)
  }

  render() {
    const {time} = this.state

    if (time === 0) {
      return null
    }

    return (
      <p>
        We were unable to send some tracked time before.<br />
        Do you want to resend {time} now?<br />
        <button onClick={reSend}>Resend</button>
      </p>
    )
  }
}
