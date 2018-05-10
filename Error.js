import React from "react"
import Layout from "./Layout"

export default class extends React.Component {
  render() {
    const {error} = this.props

    return (
      <Layout>
        <h1>Error!</h1>
        <pre>{JSON.stringify(error, null, 2)}</pre>
      </Layout>
    )
  }
}
