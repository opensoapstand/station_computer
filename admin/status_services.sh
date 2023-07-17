#!/usr/bin/env bash
echo -e "  Service Name      \t|  Running?\t|  Autostart?   "
active=$(systemctl is-active controller_soapstand.service)
enabled=$(systemctl is-enabled controller_soapstand.service)
echo -e "controller          \t| $active  \t| $enabled "
active=$(systemctl is-active ui_soapstand.service)
enabled=$(systemctl is-enabled ui_soapstand.service)
echo -e "ui                  \t| $active  \t| $enabled"
active=$(systemctl is-active check_connectivity.service)
enabled=$(systemctl is-enabled check_connectivity.service)
echo -e "periodical network check| $active  \t| $enabled"
active=$(systemctl is-active rtunnel.service)
enabled=$(systemctl is-enabled rtunnel.service)
echo -e "reversed ssh tunnel \t| $active  \t| $enabled"

CONFIG_DB_PATH=/home/df-admin/production/db/configuration.db
if [[ -f "$CONFIG_DB_PATH" ]]; then
    station_id=$(sqlite3 $CONFIG_DB_PATH "select machine_id from machine;")
    aws_port=$(sqlite3 $CONFIG_DB_PATH "select aws_port from machine;")
    software_version=$(sqlite3 $CONFIG_DB_PATH "select software_version from machine;")
    location=$(sqlite3 $CONFIG_DB_PATH "select location from machine;")
    printf "Machine id  : $station_id @ $location\nSoftware    : v$software_version\nAWS_port_db : $aws_port\n"
else
    echo "Database not found at $CONFIG_DB_PATH"
fi

port_in_use=$(sudo ./rtunnel_print.sh 2>/dev/null)

printf "rtunnel_port: $port_in_use\n"