FROM node:10
WORKDIR /usr/src/app
COPY package.json yarn.* /usr/src/app/

RUN npm config set "@fortawesome:registry" https://npm.fontawesome.com/ && \
  npm config set "//npm.fontawesome.com/:_authToken" 6093AABC-0B18-4D7C-964A-59F8E008C8B4

RUN yarn install
COPY . /usr/src/app 

EXPOSE 3000

CMD yarn start
