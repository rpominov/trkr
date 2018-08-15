import App, {Container} from "next/app"
import Layout from "components/Layout.bs"
import * as React from "react"

export default class MyApp extends App {
  render() {
    const {Component, pageProps} = this.props
    return (
      <Container>
        <Layout>
          <Component {...pageProps} />
        </Layout>
      </Container>
    )
  }
}
