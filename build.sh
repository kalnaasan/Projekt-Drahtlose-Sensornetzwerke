#! /bin/bash

docker compose -p sensor down

# Change to the DB folder and start DB for deployment
cd ./DB/
docker compose -p temp up -d

# Change to the CoAPServer folder and build the Docker image
cd ../CoAPServer/
chmod +x mvnw
./mvnw clean package -Dspring.profiles.active=test
docker build -t sensor-coap:latest --progress=plain .

# Change to the backend folder and build the Docker image
cd ../backend/
chmod +x mvnw
./mvnw clean package -Dspring.profiles.active=test
docker build -t sensor-api:latest --progress=plain .

# Change to the frontend folder and build the Docker image for the frontend
cd ../frontend
docker build -t sensor-nginx:latest --progress=plain .

# Change to the DB folder and stop DB for deployment
cd ../DB/
docker compose -p temp down

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

# Change to the main folder and start application
cd ../

# start stack
docker compose -p sensor up -d

# clean docker
docker image prune -a -f
docker volume prune -a -f