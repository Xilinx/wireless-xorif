#!/bin/bash
XROEROOTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

###############################################################################
## Address Maps
## In the default example the following base addresses are used for the 
## Radio and Framer. 
## 0xA000_0000 = roe_framer
## 0xA006_0000 = radio traffic generator
###############################################################################

###############################################################################
## Local functions
###############################################################################
USERAPPFILE=${XROEROOTDIR}/xorif-app
LOGGER="/usr/bin/logger -t $0"
LOGGER="echo"

###############################################################################
## Local functions
###############################################################################

f_testMaskedValue () {
   result=$((($1) & ($2)))
   if [ $result -gt 0 ]; then
   	  echo 1
   else
   	  echo 0
   fi
}

f_resetXxvMac () {
   ## Reset the XXV Mac, using the reset attached to the roe_framer GPIO
   devmem 0xa000000C 32 1
   devmem 0xa000000C 32 0
}

###############################################################################
## Mask and devmem read Modify Write commands
###############################################################################
f_maskValue () {
   readValue=$1
   bitWidth=$2
   bitOffset=$3

   ## Create a 32 it mask
   maskedVal=$((((((2**($bitWidth))-1) << ($bitOffset))) & ($readValue)))
   
   echo $maskedVal

}

f_readModifyWrite32 () {
   address=$1
   readValue=$(devmem $address)
   bitWidth=$2
   bitOffset=$3
   newValue=$4

   ## Create a 32 it mask
   mask=$(((~(((2**($bitWidth))-1) << ($bitOffset))) & (0xFFFFFFFF)))
   writeValue=$((($readValue) & ($mask)))
   writeValue=$((($writeValue) | (($newValue) << ($bitOffset))))
   
   ## Show the result
   #printf "@ 0x%x read:0x%x write 0x%x\n" $1  $readValue $writeValue
   devmem $address w $writeValue

}

###############################################################################
$LOGGER "**********************************************************************"
$LOGGER "** $0"
$LOGGER "** Xilinx ORAN Radio I/F demonstration platform application initscr **"
$LOGGER "**********************************************************************"

## Reset the XXV Mac, using the reset attached to the roe_framer GPIO
f_resetXxvMac

## Determine which ETH port is used for 10G and 100M Interfaces
eIf="eth0"
driverName=$(basename $(readlink  -f /sys/class/net/${eIf}/device/driver))

if [[ "$driverName" =~ xilinx_axienet ]]; then
   eth_10g="eth0"
   eth_100m="eth1"
else
   eth_10g="eth1"
   eth_100m="eth0"
fi

${LOGGER} "** Ethernet Interfaces 10G = ${eth_10g}, 10/100 = ${eth_100m}"
${LOGGER} "** Supported antennas: $(devmem 0xa0000300)"

## 
${LOGGER} "Update the AXI4 timeout so we know this has been executed"
devmem 0xa0000018 32 0x81

${LOGGER} "Switch the LED mode"
devmem 0xa0060010 32 0x1
sleep 4
devmem 0xa0060010 32 0x0

###############################################################################
dipValue=$(devmem 0xa0060014)

${LOGGER} "Bring the 10/100 link down->up"
ip link set dev ${eth_100m} down
ip link set dev ${eth_100m} up

if (( $( f_testMaskedValue $dipValue 0x02 ) != 0 )); then
   ${LOGGER} "Unit is slave assign IP address 10 on ${eth_10g}"
   xroe-config-XXV-ptp.sh 10 ${eth_10g}
   if (( $( f_testMaskedValue $dipValue 0x08 ) == 0 )); then
      ip addr add 192.168.10.120/24 dev ${eth_100m}
   fi
else
   ${LOGGER} "Unit is master assign IP address 9 on ${eth_10g}"
   xroe-config-XXV-ptp.sh 9 ${eth_10g}
   if (( $( f_testMaskedValue $dipValue 0x08 ) == 0 )); then
      ip addr add 192.168.10.100/24 dev ${eth_100m}
   fi
fi
sleep 2

if (( $( f_testMaskedValue $dipValue 0x01 ) != 0 )); then
   ${LOGGER} "Board is in LED Mode 2" 
fi

if (( $( f_testMaskedValue $dipValue 0x80 ) != 0 )); then
   ${LOGGER} "Board is in Radio Loopback mode: enable framer"
   ## This should be the Master board in the Demo. 
   devmem 0xa0002000 32 0
fi

###############################################################################
## Start the PTP demo
###############################################################################
if (( $( f_testMaskedValue $dipValue 0x04 ) != 0 )); then
   ${LOGGER} "Auto start PTP4l demo"
   if (( $( f_testMaskedValue $dipValue 0x02 ) != 0 )); then
      ${LOGGER} "Unit is PTP4l Slave"
      ptp4l -m -A -i ${eth_10g} -f ${XROEROOTDIR}/xroe-ptp4lsyncE.cfg -s &
   else
      ${LOGGER} "Unit is PTP4l Master"
      ptp4l -m -A -i ${eth_10g} &
   fi
fi

###############################################################################
## Start the xorif-app
###############################################################################
#if [ -e $USERAPPFILE ]; then
#   ${LOGGER} "Launching $USERAPPFILE -d"
#   $USERAPPFILE -d -e ${eth_10g}
#fi

###############################################################################
${LOGGER} "Bring the 10G link down->up one last time to flush system"
ip link set dev ${eth_10g} down
ip link set dev ${eth_10g} up

###############################################################################
#runTimeMonitor=${XROEROOTDIR}/xroe_monitor.pl
#if [ -e $runTimeMonitor ]; then
#   ${LOGGER} "Start our monitor apps"
#   perl -I ${XROEROOTDIR}/xroeLib ${XROEROOTDIR}/xroe_ping.pl &
#   perl -I ${XROEROOTDIR}/xroeLib ${runTimeMonitor} &
#fi

###############################################################################
## System monitor functions
##    1. Check that block lock is set, if down reset the XXV
###############################################################################
blStatus=1
blNotSetCount=0
while [ 1 ];
   do
   
   ##--------------------------------------------------------------------------
   ## Block lock check
   ##--------------------------------------------------------------------------
   blockLockStatus=$(devmem 0xa002040c)
   blockLockStatus=$(devmem 0xa002040c)

   ## Write BLOCK lock status to app scratch space in the radio
   f_readModifyWrite32 0xA0060050 1 0 $(f_maskValue $blockLockStatus 1 0) 

   if (( $( f_testMaskedValue $blockLockStatus 0x01 ) == 0 )); then
      ${LOGGER} "Block lock has been lost : $(devmem 0xa002040c) : $blockLockStatus : $blNotSetCount"
      if (($blNotSetCount > 10)); then
        ${LOGGER} "Try XXV reset."
        f_resetXxvMac
      fi 
      blStatus=0
      blNotSetCount=$((blNotSetCount + 1))
      sleep 2
   else
      if (( $blStatus == 0 )); then
         ${LOGGER} "Block lock has been reacquired : $blockLockStatus"
         blStatus=1
         blNotSetCount=0
      fi
   fi
   
   ##--------------------------------------------------------------------------
   ## Sleep
   ##--------------------------------------------------------------------------
   sleep 1

   ##--------------------------------------------------------------------------
   ## LED Heartbeat
   ##--------------------------------------------------------------------------
   ledValue=$(devmem 0xa0060010)
   if (( $( f_testMaskedValue $dipValue 0x02 ) != 0 )); then
      ## Slave flash bit 1
      let ledValue=($ledValue ^ 0x10)
   else
      ## Master flash bit 2
      let ledValue=($ledValue ^ 0x20)
   fi
   devmem 0xa0060010 32 $ledValue

done
