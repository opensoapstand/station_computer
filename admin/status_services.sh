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

db_path=/home/df-admin/production/db/drinkfill-sqlite_newlayout.db
if [[ -f "$db_path" ]]; then
    station_id=$(sqlite3 $db_path "select machine_id from machine;")
    aws_port=$(sqlite3 $db_path "select aws_port from machine;")
    software_version=$(sqlite3 $db_path "select software_version from machine;")
    location=$(sqlite3 $db_path "select location from machine;")
    printf "Machine id  : $station_id @ $location\nSoftware    : v$software_version\nAWS_port_db : $aws_port\n"
else
    echo "Database not found at $db_path"
fi

config_file="/etc/systemd/system/rtunnel.service"

# Check if the configuration file exists
if [ ! -f "$config_file" ]; then
    echo "Configuration file not found: $config_file"
    exit 1
fi

# Regex pattern to match the value to isolate
regex_pattern="^[[:space:]]*ExecStart[[:space:]]*=[[:space:]]*/usr/bin/ssh.*-R ([0-9]+):localhost:22.*"

# Read the configuration file line by line
while IFS= read -r line; do
    # Check if the line matches the regex pattern
    if [[ $line =~ $regex_pattern ]]; then
        # Extract the isolated value
        aws_port_used="${BASH_REMATCH[1]}"
    fi
done < "$config_file"
printf "rtunnel_port: $aws_port_used\n"