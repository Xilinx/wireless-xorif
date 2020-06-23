#!/bin/sh

lByte="10"

if [ ! -v $1 ]
then
   lByte=$1
else
   echo "No last byte given using ${lByte}."
fi

if [ ! -v $2 ]
then
   eTag=$2
else
   eTag="eth1"
fi

ipAddr="192.168.99.${lByte}"

echo "Setting up the XXV Ethernet link ${eTag} IP= ${ipAddr}"
ip link set dev ${eTag} up
ip addr add ${ipAddr}/24 dev ${eTag}

## Enable jumbo frames - MTU API not hooked up in driver yet
## but the driver currently forces jumbo frames on at probe() time 
#ip link set dev eth1 mtu 9000
# Turn on timestamping in AXI 10G Ethernet full time
echo "Setting timestamped mode using hwstamp_ctl..."
hwstamp_ctl -i ${eTag} -t 1 -r 1

ip addr

echo "...done!"
