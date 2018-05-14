import React from "react"
import Head from "next/head"
import Router from "next/router"
import Link from "next/link"
import Changing from "./Changing"
import Favicon from "./Favicon"

export default class Layout extends React.Component {
  state = {loading: false}

  startLoading = () => {
    this.setState({loading: true})
  }

  stopLoading = () => {
    this.setState({loading: false})
  }

  componentDidMount() {
    Router.onRouteChangeStart = this.startLoading
    Router.onRouteChangeComplete = this.stopLoading
    Router.onRouteChangeError = this.stopLoading
  }

  componentWillUnmount() {
    Router.onRouteChangeStart = null
    Router.onRouteChangeComplete = null
    Router.onRouteChangeError = null
  }

  render() {
    const {children, title} = this.props
    const {loading} = this.state
    return (
      <div className="layoutWrap">
        <Head>
          <title>{title || "TRKR"}</title>
        </Head>

        <Favicon />

        <style global jsx>{`
          body,
          html {
            font-family: monospace;
            font-size: 16px;
            line-height: 1.2;
            font-weight: normal;
          }

          * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: inherit;
            font-size: inherit;
            line-height: inherit;
            font-weight: inherit;
            color: inherit;
            background: none;
            text-decoration: none;
          }

          ul,
          li {
            display: block;
          }

          .layoutWrap {
            margin: 20px;
          }
        `}</style>

        <style jsx>{`
          h1 {
            font-weight: bold;
            font-size: 24px;
            margin-bottom: 20px;
          }
        `}</style>

        <header>
          <Link href="/">
            <a>
              <h1>
                {loading ? <Changing text="TRKR" period={100} /> : "TRKR"}
              </h1>
            </a>
          </Link>
        </header>
        <main>{children}</main>
      </div>
    )
  }
}
