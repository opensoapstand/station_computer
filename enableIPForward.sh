#!/usr/bin/env bash
ETHERNET_PORT="enp3s0"
if ip link show enp2s0|grep -q 'state UP';then
	ETHERNET_PORT="enp2s0";
	echo "In 2"
fi
if ip link show enp3s0|grep -q 'state UP';then
	ETHERNET_PORT="enp3s0"
fi
echo "${ETHERNET_PORT}"
sudo sysctl -w net.ipv4.ip_forward=1;
echo "IP forwarded";
sudo ifconfig "${ETHERNET_PORT}" "192.168.1.2";
echo "Ethernet port setup";
sudo ifconfig "${ETHERNET_PORT}" netmask 255.255.255.0;
echo "Netmask added";
sudo iptables -I FORWARD -o "${ETHERNET_PORT}" -s 192.168.0.0/16 -j ACCEPT
echo "port forwarding";
sudo iptables -I INPUT -s 192.168.0.0/16 -j ACCEPT
