#!/usr/bin/env bash
ETHERNET_PORT_ACTIVE="enp2s0"
ETHERNET_PORT_INACTIVE="enp3s0"
if ip link show enp2s0|grep -q 'state UP';then
	ETHERNET_PORT_ACTIVE="enp2s0";
	ETHERNET_PORT_INACTIVE="enp3s0";
	echo "Please change the ethernet to other port"
fi
if ip link show enp3s0|grep -q 'state UP';then
	ETHERNET_PORT_ACTIVE="enp3s0";
	ETHERNET_PORT_INACTIVE="enp2s0";
fi
echo "${ETHERNET_PORT}"
sudo iptables -t nat -A POSTROUTING -o wlo2 -j MASQUERADE
sudo sysctl -w net.ipv4.ip_forward=1;
echo "IP forwarded";
sudo ifconfig "${ETHERNET_PORT_ACTIVE}" "192.168.1.2";
# sudo ifconfig "${ETHERNET_PORT_INACTIVE}" "192.168.1.1";
echo "Ethernet port setup";
sudo ifconfig "${ETHERNET_PORT_ACTIVE}" netmask 255.255.255.0;
echo "Netmask added";
sudo iptables -I FORWARD -o "${ETHERNET_PORT_ACTIVE}" -s 192.168.0.0/16 -j ACCEPT
echo "port forwarding";
sudo iptables -I INPUT -s 192.168.0.0/16 -j ACCEPT
sudo systemctl restart ufw
echo "All completed";
