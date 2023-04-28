#!/bin/bash 
#source /proj/petalinux/2020.2/petalinux-v2020.2_daily_latest/tool/petalinux-v2020.2-final/settings.csh

## This is a global
ipRepo="-tclargs /proj/ipeng_scratch/maginnis/w/IP3_maginnis_v30l/DEV/output/internal/vivado/data/ip/xilinx/oran_radio_if_v3_0/"

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

  if [ $doVivado == 1 ]
  then 
    echo "Running Vivado    Build for $buildName"
    vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs $board -tclargs $ipMode -tclargs implNodate${exMode}Exit${extra} $ipRepo
  else
    echo "BYPASS  Vivado    Build for $buildName"
  fi

  buildName=`echo $buildName | tr [:upper:] [:lower:]`
  globalString="$buildName"
  
} 
 
function runPetalinuxBuild {

  targetBase=$1
  toolVersion=$2
  targetFull="${targetBase}_${toolVersion}"

  mkdir ../xsa/${targetBase}
  if [ $doXsaCopy == 1 ]
  then 
    echo "Copy System XSA to  ../xsa/${targetBase}"
    cp ../output/${targetFull}/vivado/system.xsa ../xsa/${targetBase}/system.xsa   
  fi
  if [ $doPetaLinux == 1 ]
  then 
    echo "Copy Stsrem XSA to  ../xsa/${targetBase}"
    make ${targetBase}   
  fi
  sleep 2
  echo "./copy_image_to_home.bash ${targetFull}"
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

##------------------------------------------------------------------------------
## Control what happens
##------------------------------------------------------------------------------
doVivado=1
doXsaCopy=1
doPetaLinux=1

##------------------------------------------------------------------------------
## Builds to run, serially.
##------------------------------------------------------------------------------
#runVpBuild zcu111 e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_cmw00i_aptw262144 Exd 2022_2 ""
#runVpBuild zcu111 e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_cmw00i_aptw262144 Exs 2022_2 "Loop"
runVpBuild zcu111 e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144        Exd 2022_2 ""
#runVpBuild zcu111 e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144        Exs 2022_2 "Loop"
#runVpBuild zcu111 e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi               Exd 2022_2 ""
#runVpBuild zcu111 e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi               Exs 2022_2 "Loop"
#runVpBuild zcu111 e4x8000x25_ss4x4_cc4x6600_dcc16x4x4x12x8_swtest             Exd 2022_2 ""
#runVpBuild zcu111 e4x8000x10_ss4x4_cc4x6600_dcc16x4x4x12x8_swtest             Exd 2022_2 ""


