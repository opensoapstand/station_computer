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
restart_rtunnel() {
    echo "Internet not reachable. Restarting the rtunnel..."
    echo 'D@nkF1ll$' | sudo -S systemctl restart $SERVICE_NAME
}

#Function to restart the network
restart_network(){
    echo "Restarting network service..."
    sudo systemctl restart network-manager
    echo "Network service restarted."
}

#Check for recent ping from AWS
checkRecentPingFromAWS(){
    database_path_usage="/home/df-admin/production/db/usage.db"
    query="SELECT time FROM events WHERE event = 'ping' ORDER BY time DESC LIMIT 1;"
    pingTime=$(sqlite3 "$database_path_usage" "$query")
    current_time=$(date +%s); 
    other_time=$(date -d "$pingTime" +%s); 
    time_difference=$((current_time - other_time)); 
    return $((time_difference/60)); 

}
check_internet_connection() {
    ping -c 1 -W 5 $PING_TARGET > /dev/null && return 0 || return 1
}


if check_internet_connection; then
    echo "Internet connection is active."
    if checkRecentPingFromAWS > 30;then
    echo "Ping not received within last 30 minutes"
    restart_rtunnel
    else
        echo "Ping received. No action required"
    fi
else
    echo "Internet is not active, Restarting network"
    restart_network
    if check_internet_connection; then
        restart_rtunnel
    fi
fi

