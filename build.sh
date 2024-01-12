#! /bin/bash

docker compose -p sensor down

# Update the repository without using the cache
git pull origin main

# Change to the CoAPServer folder and build the Docker image
cd ./CoAPServer/
chmod +x mvnw
./mvnw clean package
docker build -t sensor-coap:latest .

# Change to the backend folder and build the Docker image
cd ../backend/
chmod +x mvnw
./mvnw clean package
docker build -t sensor-api:latest .

# Change to the frontend folder and build the Docker image for the frontend
cd ../frontend
docker build -t sensor-nginx:latest .

# Name des Netzwerks
network_name="configurator_compose"

#  pruefen, ob das Netzwerk bereits vorhanden ist
if docker network inspect "$network_name" &>/dev/null; then
    echo "Das Netzwerk '$network_name' existiert bereits."
else
    # Das Netzwerk existiert nicht, daher erstellen
    docker network create --subnet 10.20.30.0/24 "$network_name"
    echo "Das Netzwerk '$network_name' wurde erfolgreich erstellt."
fi

# start stack
docker compose -p sensor up -d

# clean docker
docker image prune -a -f
docker volume prune -a -f