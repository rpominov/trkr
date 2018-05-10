import React from "react"
import Link from "next/link"
import Layout from "../Layout"
import api from "../api"
import TimeRecord from "../TimeRecord"
import {autoAuthenticate} from "../redirect"

const INCREMENT_BY = 10

function getTrkrFieldId(board) {
  const trkrField = board.customFields.filter(f => f.name === "trkr")[0]
  return trkrField ? trkrField.id : null
}

function getTrkrFieldValue(card, fieldId) {
  const field = card.customFieldItems.filter(
    f => f.idCustomField === fieldId,
  )[0]
  return field ? field.value.text : ""
}

function getAllTrkrFieldValues(cards, fieldId) {
  const result = {}
  cards.forEach(card => {
    result[card.id] = getTrkrFieldValue(card, fieldId)
  })
  return result
}

export default class extends React.Component {
  static async getInitialProps(context) {
    const board = await api(context, `boards/${context.query.id}`, {
      fields: "name,prefs",
      customFields: "true",
    })

    if (autoAuthenticate(board, context)) {
      return {}
    }

    if (board.tag === "error") {
      return {error: board}
    }

    const trkrFieldId = getTrkrFieldId(board.result)

    if (trkrFieldId === null) {
      return {board: board.result, trkrFieldId}
    }

    const cards = await api(context, `boards/${context.query.id}/cards/open`, {
      customFieldItems: "true",
      fields: "name,shortUrl,idList",
    })

    if (autoAuthenticate(cards, context)) {
      return {}
    }

    if (cards.tag === "error") {
      return {error: cards}
    }

    return {board: board.result, cards: cards.result, trkrFieldId}
  }

  state = {
    currentCard: null,
    trkrFieldValues: this.props.trkrFieldId
      ? getAllTrkrFieldValues(this.props.cards, this.props.trkrFieldId)
      : {},
  }

  componentDidMount() {
    this.intervalId = setInterval(this.increment, INCREMENT_BY * 1000)
  }

  componentWillUnmount() {
    clearInterval(this.intervalId)
  }

  async updateTrkrField(cardId, newValue) {
    const {trkrFieldId} = this.props

    await api(
      null,
      `card/${cardId}/customField/${trkrFieldId}/item`,
      {},
      "PUT",
      {
        value: {text: newValue},
      },
    )

    this.setState(s => ({
      trkrFieldValues: {...s.trkrFieldValues, [cardId]: newValue},
    }))
  }

  increment = async () => {
    const {currentCard} = this.state
    const {trkrFieldId} = this.props

    if (currentCard === null) {
      return
    }

    const card = await api(null, `cards/${currentCard}`, {
      customFieldItems: "true",
      fields: "name",
    })

    if (autoAuthenticate(card)) {
      return
    }

    if (card.tag === "error") {
      // TODO: save localy?
      // TODO: show error in UI?
      return
    }

    const newValue = TimeRecord.stringify(
      TimeRecord.increment(
        TimeRecord.parse(getTrkrFieldValue(card.result, trkrFieldId)),
        INCREMENT_BY,
      ),
    )

    await this.updateTrkrField(currentCard, newValue)
  }

  render() {
    const {cards, board, trkrFieldId} = this.props
    const {currentCard, trkrFieldValues} = this.state

    const color =
      board.prefs.backgroundBrightness === "dark" ? "white" : "black"

    const textBackground =
      board.prefs.backgroundBrightness === "dark"
        ? "rgba(0,0,0,0.5)"
        : "rgba(255,255,255,0.5)"

    const backgroundImage = board.prefs.backgroundImage
      ? `url(${board.prefs.backgroundImageScaled[1].url})`
      : null

    const backgroundColor = board.prefs.backgroundImage
      ? "#666"
      : board.prefs.background

    return (
      <Layout>
        <style jsx>{`
          h2 {
            margin: 0 0 40px 0;
          }

          h2 span {
            padding: 4px 8px;
            font-size: 30px;
          }

          h2 div {
            background-size: cover;
            background-position: center;
            width: 100px;
            height: 100px;
            margin-bottom: -39px;
          }

          li {
            margin-bottom: 20px;
            position: relative;
          }

          li input[type="radio"] {
            position: absolute;
          }

          li div {
            margin-left: 22px;
            margin-top: 2px;
          }

          li p {
            color: #9a9a9a;
            font-size: 12px;
          }

          @keyframes blink-animation {
            0%,
            100% {
              opacity: 1;
            }
            50% {
              opacity: 0.3;
            }
          }

          .blink {
            animation: blink-animation 1s linear infinite;
          }
        `}</style>

        <h2>
          <div style={{backgroundImage, backgroundColor}} />
          <span style={{color, background: textBackground}}>{board.name}</span>
        </h2>

        {trkrFieldId ? (
          <ul>
            <li>
              <input
                type="radio"
                name="current-card"
                checked={currentCard === null}
                onChange={() => this.setState({currentCard: null})}
                id="current-card-radio__rest"
              />
              <div>
                <label htmlFor="current-card-radio__rest">Rest</label>
              </div>
            </li>
            {cards.map(x => {
              const elId = "current-card-radio__" + x.id
              const isCurrent = currentCard === x.id
              const time = TimeRecord.formatTodayRest(trkrFieldValues[x.id])
              const onChange = () => this.setState({currentCard: x.id})
              return (
                <li key={x.id}>
                  <input
                    type="radio"
                    name="current-card"
                    checked={isCurrent}
                    onChange={onChange}
                    id={elId}
                  />
                  <div>
                    <label htmlFor={elId}>{x.name}</label>
                    <p>{time}</p>
                  </div>
                </li>
              )
            })}
          </ul>
        ) : (
          'Create the "trkr" custom field in the board before you can use it with TRKR.'
        )}
      </Layout>
    )
  }
}
