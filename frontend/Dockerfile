#############
### build ###
#############

# base image
FROM node:18.18.0 as build

# set working directory
WORKDIR /app

# add `/app/node_modules/.bin` to $PATH
ENV PATH /app/node_modules/.bin:$PATH

# install and cache app dependencies
COPY package.json /app/package.json
COPY package-lock.json /app/package-lock.json
RUN npm install
#RUN npm audit fix
RUN npm install -g @angular/cli@16.2.4
# has to be automated for future work

# add app
COPY . /app

# generate build
RUN ng build --output-path=dist --configuration=production

############
### prod ###
############

# base image
FROM nginx

## Remove default Nginx website
RUN rm -rf /usr/share/nginx/html/*
COPY ./nginx.conf /etc/nginx/nginx.conf

# copy artifact build from the 'build environment'
COPY --from=build /app/dist /usr/share/nginx/html

# run nginx
CMD ["nginx", "-g", "daemon off;"]
