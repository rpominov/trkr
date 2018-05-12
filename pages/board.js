import React from "react"
import Link from "next/link"
import Layout from "../Layout"
import Error from "../Error"
import {ApiMonad} from "../api"
import {readCookie, writeCookie} from "../cookies"
import {autoAuthenticate} from "../redirect"
import TimeRecord from "../TimeRecord"
import Blink from "../Blink"

const INCREMENT_BY = 10
const COLLAPSED_LISTS = "coli"

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
    return ApiMonad.doAndHandleError(context, function*() {
      const boardId = context.query.id

      const board = yield ApiMonad.call([
        `boards/${boardId}`,
        {fields: "name,prefs", customFields: "true"},
      ])

      const trkrFieldId = getTrkrFieldId(board)

      if (trkrFieldId === null) {
        return {board, trkrFieldId}
      }

      const lists = yield ApiMonad.call([
        `boards/${boardId}/lists`,
        {fields: "name"},
      ])

      lists.reverse()

      const cards = yield ApiMonad.call([
        `boards/${boardId}/cards/open`,
        {customFieldItems: "true", fields: "name,shortUrl,idList"},
      ])

      const cookies = readCookie(context)
      const initialCollapsedLists = cookies[COLLAPSED_LISTS]
        ? cookies[COLLAPSED_LISTS].split("-")
        : []

      return {board, cards, lists, trkrFieldId, initialCollapsedLists}
    })
  }

  state = {
    currentCard: null,
    trkrFieldValues: this.props.trkrFieldId
      ? getAllTrkrFieldValues(this.props.cards, this.props.trkrFieldId)
      : {},
    collapsedLists: this.props.initialCollapsedLists,
  }

  componentDidMount() {
    this.intervalId = setInterval(this.increment, INCREMENT_BY * 1000)
  }

  componentWillUnmount() {
    clearInterval(this.intervalId)
  }

  increment = async () => {
    const {currentCard} = this.state
    const {trkrFieldId} = this.props

    if (currentCard === null) {
      return
    }

    const result = await ApiMonad.run(
      ApiMonad.do(function*() {
        const card = yield ApiMonad.call([
          `cards/${currentCard}`,
          {
            customFieldItems: "true",
            fields: "name",
          },
        ])

        const newValue = TimeRecord.stringify(
          TimeRecord.increment(
            TimeRecord.parse(getTrkrFieldValue(card, trkrFieldId)),
            INCREMENT_BY,
          ),
        )

        yield ApiMonad.call([
          `card/${currentCard}/customField/${trkrFieldId}/item`,
          {},
          "PUT",
          {
            value: {text: newValue},
          },
        ])

        return newValue
      }),
    )

    if (autoAuthenticate(result)) {
      return
    }

    if (result.tag === "error") {
      // TODO: save localy?
      // TODO: show error in UI?
      // like "couldn't track 3.43 hours [send now]"
      alert("couldnt track!")
      return
    }

    this.setState(s => ({
      trkrFieldValues: {...s.trkrFieldValues, [currentCard]: result.result},
    }))
  }

  setCollapsed(listId, collapsed) {
    const {collapsedLists} = this.state
    const newCollapsedLists = collapsed
      ? collapsedLists.concat(listId)
      : collapsedLists.filter(x => x !== listId)
    this.setState({collapsedLists: newCollapsedLists})
    writeCookie({
      name: COLLAPSED_LISTS,
      value: newCollapsedLists.join("-"),
      maxAge: 60 * 60 * 24 * 356,
    })
  }

  render() {
    if (this.props.error) {
      return <Error error={this.props} />
    }

    const {cards, board, trkrFieldId, lists} = this.props
    const {currentCard, trkrFieldValues, collapsedLists} = this.state

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

    const title =
      currentCard === null
        ? "Resting..."
        : TimeRecord.formatToday(trkrFieldValues[currentCard]) +
          "h @ " +
          cards.find(x => x.id === currentCard).name

    return (
      <Layout title={title}>
        <style jsx>{`
          h2 {
            margin: 0 0 40px 0;
          }

          h2 span {
            padding: 4px 8px;
            font-size: 30px;
            line-height: 30px;
            display: inline-block;
          }

          h2 div {
            background-size: cover;
            background-position: center;
            width: 100px;
            height: 100px;
            margin-bottom: -38px;
          }

          li {
            margin-bottom: 20px;
            position: relative;
          }

          li input[type="radio"] {
            position: absolute;
            font-size: 14px;
            margin-top: 2px;
            cursor: pointer;
          }

          li label {
            cursor: pointer;
          }

          li div {
            margin-left: 22px;
          }

          li p {
            color: #9a9a9a;
            font-size: 12px;
          }

          .list-wrap {
            margin-bottom: 20px;
          }

          .toggle-collapse {
            float: right;
            cursor: pointer;
            border: none;
            font-size: 14px;
          }

          h3 {
            background: #87bdf5;
            color: white;
            margin-bottom: 10px;
          }
        `}</style>

        <h2>
          <div style={{backgroundImage, backgroundColor}} />
          <span style={{color, background: textBackground}}>{board.name}</span>
        </h2>

        {trkrFieldId && (
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
                <label htmlFor="current-card-radio__rest">Rest...</label>
              </div>
            </li>
          </ul>
        )}

        {trkrFieldId &&
          lists.map(l => {
            const collapsed = collapsedLists.indexOf(l.id) !== -1
            return (
              <div key={l.id} className="list-wrap">
                <h3>
                  {l.name}{" "}
                  <button
                    className="toggle-collapse"
                    onClick={() => this.setCollapsed(l.id, !collapsed)}
                  >
                    {collapsed ? "[expand]" : "[collapse]"}
                  </button>
                </h3>
                {!collapsed && (
                  <ul>
                    {cards.filter(x => x.idList === l.id).map(x => {
                      const elId = "current-card-radio__" + x.id
                      const isCurrent = currentCard === x.id
                      const time = TimeRecord.formatTodayRest(
                        trkrFieldValues[x.id],
                      )
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
                            <p>
                              {isCurrent
                                ? time.map(
                                    (x, i) =>
                                      typeof x === "number" ? (
                                        <Blink key={i}>{x}</Blink>
                                      ) : (
                                        x
                                      ),
                                  )
                                : time}
                            </p>
                          </div>
                        </li>
                      )
                    })}
                  </ul>
                )}
              </div>
            )
          })}

        {!trkrFieldId &&
          'Create the "trkr" custom field in the board before you can use it with TRKR.'}
      </Layout>
    )
  }
}
