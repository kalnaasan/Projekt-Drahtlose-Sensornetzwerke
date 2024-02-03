#! /bin/bash

if [ "$(id -u)" != "0" ]; then
    echo "Dieses Skript erfordert Superuser-Rechte. Bitte fuehren Sie es mit sudo aus."
    exit 1
fi

# Name des Netzwerks
network_name="compose"

#  pruefen, ob das Netzwerk bereits vorhanden ist
if docker network inspect "$network_name" &>/dev/null; then
    echo "Das Netzwerk '$network_name' existiert bereits."
else
    # Das Netzwerk existiert nicht, daher erstellen
    docker network create --subnet 10.20.30.0/24 "$network_name"
    echo "Das Netzwerk '$network_name' wurde erfolgreich erstellt."
fi

# stop stack if exisit
set +e
docker compose -p sensor down
set +e

# Change to the DB folder and start DB for deployment
cd ./DB/
docker compose -p db up -d

# Funktion zur ueberpruefen, ob MySQL bereit ist
check_mysql() {
  docker exec mysql mysqladmin ping -hmysql -u root --password=mysql &> /dev/null
}

# Warten, bis MySQL bereit ist
while ! check_mysql; do
  echo "MySQL ist nicht bereit. Warte 5 Sekunden..."
  sleep 5
done

echo "MySQL ist bereit!"

# Change to the CoAPServer folder and build the Docker image
cd ../CoAPServer/
chmod +x mvnw
./mvnw clean package -Dspring.profiles.active=test
docker build -t sensor-coap:latest .

# Change to the backend folder and build the Docker image
cd ../backend/
chmod +x mvnw
./mvnw clean package -Dspring.profiles.active=test
docker build -t sensor-api:latest .

# Change to the frontend folder and build the Docker image for the frontend
cd ../frontend
docker build -t sensor-nginx:latest .

# Change to the main folder and start application
cd ..

# start stack
docker compose -p sensor up -d

# clean docker
docker image prune -a -f
docker volume prune -a -f
