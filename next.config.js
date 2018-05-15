module.exports = {
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

    return config
  },
}
