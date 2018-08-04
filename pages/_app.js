import App, {Container} from "next/app"
import Layout from "../Layout"
import * as React from "react"

export default class MyApp extends App {
  render() {
    const {Component, pageProps, router} = this.props
    return (
      <Container>
        <Layout>
          <Component {...pageProps} router={router} />
        </Layout>
      </Container>
    )
  }
}
