#!/bin/bash

# do network set-up
sudo ip link add eth1 type dummy
sudo ifconfig eth1 hw ether C8:D7:4A:4E:47:50
sudo ip address add 192.168.0.1/24 dev eth1
sudo ip address add 192.168.0.2/24 dev eth1
sudo ip link set dev eth1 up

printf "Network Setup Completed \n"