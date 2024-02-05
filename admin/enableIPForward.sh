#!/usr/bin/env bash

ETHERNET_PORT_ACTIVE="enp2s0"
ETHERNET_PORT_INACTIVE="enp3s0"

# Check which Ethernet port is active
if ip link show enp2s0 | grep -q 'state UP'; then
    ETHERNET_PORT_ACTIVE="enp2s0"
    ETHERNET_PORT_INACTIVE="enp3s0"
    echo "Please change the Ethernet to the other port"
fi

if ip link show enp3s0 | grep -q 'state UP'; then
    ETHERNET_PORT_ACTIVE="enp3s0"
    ETHERNET_PORT_INACTIVE="enp2s0"
fi

# Enable IP forwarding persistently
echo "net.ipv4.ip_forward=1" | sudo tee /etc/sysctl.d/99-ip_forward.conf >/dev/null
sudo sysctl -p

# Save iptables rules persistently
sudo iptables -t nat -A POSTROUTING -o wlo2 -j MASQUERADE
sudo iptables -A FORWARD -o "${ETHERNET_PORT_ACTIVE}" -s 192.168.0.0/16 -j ACCEPT
sudo iptables -A INPUT -s 192.168.0.0/16 -j ACCEPT

# Create directory if it doesn't exist
sudo mkdir -p /etc/iptables

# Save iptables rules to file
sudo iptables-save | sudo tee /etc/iptables/rules.v4 >/dev/null

# Configure network interfaces persistently
sudo tee /etc/network/interfaces <<EOF
auto $ETHERNET_PORT_ACTIVE
iface $ETHERNET_PORT_ACTIVE inet static
    address 192.168.1.2
    netmask 255.255.255.0
EOF

# Restart network service to apply changes
sudo systemctl restart networking

# Restart UFW (Uncomplicated Firewall) if needed
sudo systemctl restart ufw

echo "Configuration completed successfully."
