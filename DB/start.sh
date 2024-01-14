#!/bin/bash

# Docker Compose starten
docker compose up -d

# Funktion zur Überprüfung, ob MySQL bereit ist
check_mysql() {
  docker exec mysql mysqladmin ping -hmysql -u root --password=mysql &> /dev/null
}

# Warten, bis MySQL bereit ist
while ! check_mysql; do
  echo "MySQL ist nicht bereit. Warte 5 Sekunden..."
  sleep 5
done

echo "MySQL ist bereit!"
