#!/bin/bash 
#source /proj/petalinux/2020.2/petalinux-v2020.2_daily_latest/tool/petalinux-v2020.2-final/settings.csh

## This is a global
ipRepo="-tclargs /proj/ipeng/staff/maginnis/iiippp/IP3_maginnis_oran_radio_if_1_1_DEV_boot/DEV/output/internal/vivado/data/ip/xilinx/oran_radio_if_v1_1/"

## This is a serial script. Run over night. If the config list gets too big, parallelise

## Use a global for return values. Return from functions can only be an integer.
## If we echo and capture we lose the active output,
## Be very careful if you nest.
globalString=""

function runVivadoBuild {

  board=$1
  ipMode=$2
  exMode=$3
  extra=$4
  buildName="${board}_${ipMode}_${exMode}"

  echo "Running Vivado    Build for $buildName"

  vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs $board -tclargs $ipMode -tclargs implNodate${exMode}Exit${extra} $ipRepo

  buildName=`echo $buildName | tr [:upper:] [:lower:]`
  globalString="$buildName"
  
} 
 
function runPetalinuxBuild {

  targetBase=$1
  toolVersion=$2
  targetFull="${targetBase}_${toolVersion}"

  mkdir ../xsa/${targetBase}
  cp ../output/${targetFull}/vivado/${targetFull}.sdk/${targetFull}.xsa ../xsa/${targetBase}/system.xsa   
  make ${targetBase}
  ./copy_image_to_home.bash ${targetFull}

  echo "Petalinux done for ${targetFull}"

} 
 
function runVpBuild {

  board=$1
  ipMode=$2
  exMode=$3
  toolVersion=$4

  runVivadoBuild ${board} ${ipMode} ${exMode}
  buildName=$globalString
  echo "Running Petalinux Build for $buildName"
  runPetalinuxBuild $buildName $toolVersion

} 
    
#runVpBuild zcu111 om5        Exs 2020_2 ""
#runVpBuild zcu111 om5_25     Exs 2020_2 ""
#runVpBuild zcu111 om5_10_slv Exd 2020_2 ""
runVpBuild zcu102 om5        Exs 2020_2 Loop

