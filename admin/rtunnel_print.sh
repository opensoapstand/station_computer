#!/usr/bin/env bash
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
echo "$aws_port_used"
# printf "rtunnel_port: $aws_port_used\n"