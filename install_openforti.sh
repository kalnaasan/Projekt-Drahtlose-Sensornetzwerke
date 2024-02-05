#!/bin/bash

# Überprüfe, ob der Benutzer Root-Rechte hat
if [ "$EUID" -ne 0 ]; then
  echo "Dieses Skript erfordert Root-Rechte. Bitte führe es mit sudo aus."
  exit 1
fi

# Überprüfe, ob OpenFortiVPN installiert ist
if ! command -v openfortivpn &> /dev/null; then
    echo "OpenFortiVPN ist nicht installiert. Installiere es jetzt..."
    apt-get update
    apt-get install -y openfortivpn
fi

# Benutzer nach VPN-Konfigurationsdetails fragen
read -p "Gib den VPN-Host ein: " vpn_host
read -p "Gib den Port ein (Standard ist 443): " vpn_port
read -p "Gib den Realm ein: " vpn_realm

read -p "Gib den Benutzernamen ein: " vpn_username
read -s -p "Gib das Passwort ein: " vpn_password
echo  # Neue Zeile für die Sichtbarkeit

# Überprüfe, ob IPv6 aktiviert ist
if grep -q '^net.ipv6.conf.all.disable_ipv6 = 0' /etc/sysctl.conf; then
    echo "IPv6 ist bereits aktiviert."
else
    echo "IPv6 ist deaktiviert. Aktiviere IPv6..."

    # Aktiviere IPv6 in der sysctl.conf
    echo 'net.ipv6.conf.all.disable_ipv6 = 0' >> /etc/sysctl.conf

    # Wende die Änderungen an
    sysctl -p

    echo "IPv6 wurde aktiviert."
fi

# Pfad zur OpenFortiVPN-Konfigurationsdatei
config_file="/etc/openfortivpn/config"

# OpenFortiVPN-Konfigurationsdatei erstellen oder aktualisieren
echo "#### config file for openfortivpn, see man openfortivpn(1)" > "$config_file"
echo "###" >> "$config_file"
echo "#" >> "$config_file"
echo "# host = Zieladresse des VPN-Gateways" >> "$config_file"
echo "# port = Zielport" >> "$config_file"
echo "# realm = Bereich" >> "$config_file"
echo "# username = Username" >> "$config_file"
echo "# password = Passwort" >> "$config_file"
echo "# ca-file = Zertifikatskette" >> "$config_file"
echo "host = $vpn_host" >> "$config_file"
echo "port = ${vpn_port:-443}" >> "$config_file"
echo "ipv6 = true" >> "$config_file"
echo "realm = $vpn_realm" >> "$config_file"
echo "username = $vpn_username" >> "$config_file"
echo "password = $vpn_password" >> "$config_file"

# Systemd-Dienstkonfiguration erstellen oder aktualisieren
service_file="/etc/systemd/system/openfortivpn.service"
echo "[Unit]" > "$service_file"
echo "Description=OpenFortiVPN Client Service" >> "$service_file"
echo "" >> "$service_file"
echo "[Service]" >> "$service_file"
echo "Type=simple" >> "$service_file"
echo "ExecStart=/usr/bin/openfortivpn -c $config_file" >> "$service_file"
echo "" >> "$service_file"
echo "[Install]" >> "$service_file"
echo "WantedBy=default.target" >> "$service_file"

# Dienst neu starten und aktivieren
systemctl restart openfortivpn
systemctl enable openfortivpn
systemctl daemon-reload

echo "OpenFortiVPN wurde konfiguriert und der Dienst wurde neu gestartet."

exit 0