const withCSS = require("@zeit/next-css")
const path = require('path')

module.exports = withCSS({
  cssModules: true,

  publicRuntimeConfig: {
    appKey: process.env.APP_KEY,
  },

  webpack(config) {
    config.module.rules.push({
      test: /\.(png)$/,
      use: {
        loader: "url-loader",
        options: {
          limit: 100000,
          publicPath: "./",
          outputPath: "static/",
          name: "[name].[ext]",
        },
      },
    })

    config.resolve.modules.push(
      path.resolve(__dirname, './lib/js/src'),
      path.resolve(__dirname, './src'),
      path.resolve(__dirname)
    )

    return config
  },
})
