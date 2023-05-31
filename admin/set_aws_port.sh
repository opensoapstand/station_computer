#!/usr/bin/env bash

# script will modify the aws_port in the db, change it in the service file and reload services.

is_valid_port() {
    # Function to check if a value is a valid number of 5 digits
    re='^[0-9]+$'
    if ! [[ $1 =~ $re ]]; then
        return 1
    fi
    if [ ${#1} -ne 5 ]; then
        return 1
    fi
    return 0
}

database_path="/home/df-admin/production/db/drinkfill-sqlite_newlayout.db"
query="Select aws_port from machine"
port=$(sqlite3 "$database_path" "$query")

# Ask user for input
while true; do
    read -e -p "Enter a port number [old: $port]: " -i "$port" new_port

    # Check if the input is a valid number of 5 digits
    if is_valid_port "$new_port"; then
        echo "Entered port: $new_port"
        break
    else
        echo "Invalid input. Please enter a valid number of 5 digits."
    fi
done

    # Ask for confirmation
    read -p "Are you sure you want to set the machine to $new_port? [y/n]: " choice

    if [[ $choice == "y" || $choice == "Y" ]]; then
        # Update db
        query="update machine set aws_port=$new_port;"
        port=$(sqlite3 "$database_path" "$query")
    else
        echo "User cancelled. Port stays at $port"
        exit 0
    fi

    #########################################
    # update service

    # Path to the configuration file
    config_file="/home/df-admin/production/admin/rtunnel.service"

    modified_contents=""
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
            old_port="${BASH_REMATCH[1]}"

            # Replace the value using sed
            modified_line="${line//$old_port/$new_port}"
            modified_contents+="$modified_line\n"
        else
            # Append the line to the modified contents
            modified_contents+="$line\n"
        fi
    done < "$config_file"

    # # Overwrite the original config file with the modified contents
    echo -e "$modified_contents" > "$config_file"

sudo scp /home/df-admin/production/admin/rtunnel.service /etc/systemd/system
sudo systemctl daemon-reload
sudo service rtunnel restart
echo "done"

