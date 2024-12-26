#!/bin/bash

# do network set-up
ip link add eth1 type dummy
ifconfig eth1:0 172.16.25.127
ifconfig eth1:1 172.16.25.128
ifconfig eth1:2 172.16.25.129
ip link set dev eth1 up

printf "Network Setup Completed \n"
