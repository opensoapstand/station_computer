#!/usr/bin/env bash
# echo "controller:"
# systemctl is-active controller_soapstand.service
# systemctl is-enabled controller_soapstand.service
# echo "ui:"
# systemctl is-active ui_soapstand.service
# systemctl is-enabled ui_soapstand.service
# echo "reversed ssh tunnel:"
# systemctl is-active rtunnel.service
# systemctl is-enabled rtunnel.service
# systemctl status controller_soapstand.service
# systemctl status ui_soapstand.service
# systemctl status rtunnel.service
echo -e "  Service Name      |  Running?\t|  Autostart?   "
active=$(systemctl is-active controller_soapstand.service)
enabled=$(systemctl is-enabled controller_soapstand.service)
echo -e "controller          |$active  \t|$enabled "
active=$(systemctl is-active ui_soapstand.service)
enabled=$(systemctl is-enabled ui_soapstand.service)
echo -e "ui                  |$active  \t|$enabled"
active=$(systemctl is-active rtunnel.service)
enabled=$(systemctl is-enabled rtunnel.service)
echo -e "reversed ssh tunnel |$active  \t|$enabled"

db_path=/home/df-admin/production/db/drinkfill-sqlite_newlayout.db
if [[ -f "$db_path" ]]; then
    station_id=$(sqlite3 $db_path "select machine_id from machine;")
    software_version=$(sqlite3 $db_path "select software_version from machine;")
    location=$(sqlite3 $db_path "select location from machine;")
    printf "Machine id : $station_id @ $location\nSoftware   : v$software_version\n"
else
    echo "Database not found at $db_path"
fi
