#!/bin/sh

# Get driver name for eth0
driver=$(basename $(readlink -f /sys/class/net/eth0/device/driver))

if [[ "$driver" == xilinx_axienet ]]; then
	# We want the GEM to be eth0, so swap the ports
	# Set both eth0 and eth1 ports to "down"
	ifdown eth0
	ifdown eth1

	# Swap the port names (using "ethTmp" as a temporary name)
	ip link set dev eth0 name ethTmp
	ip link set dev eth1 name eth0
	ip link set dev ethTmp name eth1
	 
	# Set both eth0 and eth1 ports to "up"
	ifup eth0
	ifup eth1
fi