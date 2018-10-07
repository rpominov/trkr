FROM node:latest
WORKDIR /app
COPY . .
ENV PORT=80
ENV APP_KEY=<...>
EXPOSE 80
RUN npm install
RUN npm run heroku-postbuild
CMD ["npm", "start"]