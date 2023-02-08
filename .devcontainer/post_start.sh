#!/bin/bash

# do network set-up
sudo ip link add eth1 type dummy
# sudo ip link add eth2 type dummy
# sudo ifconfig eth1 hw ether C8:D7:4A:4E:47:50
# sudo ifconfig eth2 hw ether C8:D7:4A:4E:47:60
# sudo ifconfig eth1 192.168.2.1 netmask 255.255.255.0
# sudo ip address add 192.168.2.2/24 dev eth1
# sudo ifconfig eth2 192.168.2.3 netmask 255.255.255.0
ifconfig eth1:0 172.16.25.127
ifconfig eth1:1 172.16.25.128
sudo ip link set dev eth1 up

printf "Network Setup Completed \n"