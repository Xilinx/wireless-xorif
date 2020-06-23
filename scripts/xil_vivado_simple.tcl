## Load the latest version
## module load xilinx/ta/2020.1_daily_latest

## 
proc xorif_test_exdes { PROJECT } {

  set PROJECT "${PROJECT}_ip"
  create_project -force $PROJECT $PROJECT -part xczu28dr-ffvg1517-2-e
  set_property board_part xilinx.com:zcu111:part0:1.2 [current_project]

  ## You can add custom repo paths
  #set_property  ip_repo_paths  /home/${HOME}/random/ip/hier/repo/ [current_project]
  #update_ip_catalog

  create_bd_design "design_1"
  
  open_bd_design [get_files design_1.bd]
  
  create_bd_cell -type ip -vlnv xilinx.com:ip:oran_radio_if:1.0 oran_radio_if_0
  save_bd_design
  
  open_example_project -force -dir ${PROJECT}_exdes [get_ips]
  
}

proc xorif_test_ba { PROJECT } {

  set PROJECT "${PROJECT}_ba"
  create_project -force $PROJECT $PROJECT -part xczu28dr-ffvg1517-2-e
  set_property board_part xilinx.com:zcu111:part0:1.2 [current_project]

  ## You can add custom repo paths
  #set_property  ip_repo_paths  /home/${HOME}/random/ip/hier/repo/ [current_project]
  #update_ip_catalog

  create_bd_design "design_1"
  
  open_bd_design [get_files design_1.bd]
  
  create_bd_cell -type ip -vlnv xilinx.com:ip:oran_radio_if:1.0 oran_radio_if_0
  save_bd_design
  
  apply_bd_automation -rule xilinx.com:bd_rule:oran_radio_if -config { cfg_addBlFifoReset {1} processorSelect {ARM_Linux} ptpSelect {XilinxPtpV1}}  [get_bd_cells oran_radio_if_0]
  
}


proc show_help {} {
  puts "
################################################################################
## Simple ORAN Radio Interface TCL examples
## The TCL procs in this file are simple examples of how to generate both the 
## Example Design and Example System using TCL.
## The parameterisation can go much further, but this should give a simple intro
## to the basics that can be run.
################################################################################

##--------------------- Block automation
set PROJECT prj3
xorif_test_ba \$PROJECT

##--------------------- Example design
xorif_test_exdes prj2

##--------------------- Clean up using
rm -r vivado* prj*

################################################################################
"

}

show_help
