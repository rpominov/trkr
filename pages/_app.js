import App, {Container} from "next/app"
import Layout from "../Layout"
import * as React from "react"

export default class MyApp extends App {
  static async getInitialProps({Component, router, ctx}) {
    let pageProps = {}

    if (Component.getInitialProps) {
      pageProps = await Component.getInitialProps(ctx)
    }

    return {pageProps}
  }

  render() {
    const {Component, pageProps, router} = this.props
    return (
      <Container>
        <Layout>
          <Component {...pageProps} />
        </Layout>
      </Container>
    )
  }
}
