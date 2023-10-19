#!/usr/bin/env bash

database_path="/home/df-admin/production/db/configuration.db"
query="Select payment from products where slot=1"
paymentMethod=$(sqlite3 "$database_path" "$query")

# Define the expected payment method
EXPECTED_PAYMENT_METHOD="tapTCP"

# Get the current payment method (replace this with your method)
CURRENT_PAYMENT_METHOD="$(sqlite3 "$database_path" "$query")"

if [ "$CURRENT_PAYMENT_METHOD" = "$EXPECTED_PAYMENT_METHOD" ]; then
    ETHERNET_PORT_ACTIVE="enp3s0"
    
    echo "${ETHERNET_PORT_ACTIVE}"
    sudo sysctl -w net.ipv4.ip_forward=1
    echo "IP forwarded"
    sudo ifconfig "${ETHERNET_PORT_ACTIVE}" "192.168.1.2"
    #sudo ifconfig "${ETHERNET_PORT_INACTIVE}" "192.168.1.1"
    echo "Ethernet port setup"
    sudo ifconfig "${ETHERNET_PORT_ACTIVE}" netmask 255.255.255.0
    echo "Netmask added"
    sudo iptables -I FORWARD -o "${ETHERNET_PORT_ACTIVE}" -s 192.168.0.0/16 -j ACCEPT
    echo "Port forwarding"
    sudo iptables -I INPUT -s 192.168.0.0/16 -j ACCEPT
    sudo systemctl restart ufw
    echo "All completed"
else
    echo "Payment method does not match. Script not executed."
fi