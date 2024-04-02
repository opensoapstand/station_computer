#!/usr/bin/env bash

ETHERNET_PORT_ACTIVE="enp2s0"
ETHERNET_PORT_INACTIVE="enp3s0"

if ip link show enp2s0 | grep -q 'state UP'; then
    ETHERNET_PORT_ACTIVE="enp2s0"
    ETHERNET_PORT_INACTIVE="enp3s0"
    echo "In 2"
fi
if ip link show enp3s0 | grep -q 'state UP'; then
    ETHERNET_PORT_ACTIVE="enp3s0"
    ETHERNET_PORT_INACTIVE="enp2s0"
fi

# Flush iptables rules
sudo iptables -F
sudo iptables -t nat -F
sudo iptables -X
sudo iptables -t nat -X
sudo iptables -t mangle -F
sudo iptables -t mangle -X
sudo iptables -P INPUT ACCEPT
sudo iptables -P FORWARD ACCEPT
sudo iptables -P OUTPUT ACCEPT

echo ${ETHERNET_PORT_ACTIVE}

# Reset network configurations
sudo ifconfig "${ETHERNET_PORT_ACTIVE}" 0.0.0.0
sudo ifconfig "${ETHERNET_PORT_ACTIVE}" down
sudo ifconfig "${ETHERNET_PORT_ACTIVE}" up

# Remove certain files
sudo rm /etc/iptables/rules.v4
sudo rm /etc/network/interfaces

echo "Reverted IP forwarding changes."
