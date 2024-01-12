#!/bin/bash

# Set your ping target (Google's public DNS server)
PING_TARGET="8.8.8.8"

# Set your service details
SERVICE_NAME="rtunnel.service"

# Function to check internet connectivity
check_internet_connection() {
    ping -c 1 -W 5 $PING_TARGET > /dev/null && return 0 || return 1
}

# Function to restart the service
restart_service() {
    echo "Internet not reachable. Restarting the service..."
    echo 'D@nkF1ll$' | sudo -S systemctl restart $SERVICE_NAME
}

if check_internet_connection; then
    echo "Internet connection is active."
else
    echo "Internet is not active, Restarting rtunnel"
    restart_service
fi
