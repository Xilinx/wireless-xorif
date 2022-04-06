## ----------------------------------------------------------------------------
## Vivado TCL script to help build ROE Framer Example Systems. 
## ----------------------------------------------------------------------------
namespace eval ::roe::data {
  
  ## --------------------------------------------------------------------------
  ## Return a Dict that contains all the settings we need, IP, Parts and 
  ## automation types.
  ## --------------------------------------------------------------------------
  package require yaml

  proc readYamlFileRetDict { fileNameIn } {
    set fp [open $fileNameIn r]
    set file_data [read $fp]
    close $fp
    return [yaml::yaml2dict $file_data]
  }

  proc return_config_dict { } {
    set settingsFile "xil_vivado_configs.yml"
    puts "Loading settings from $settingsFile"
    set cfg [readYamlFileRetDict $settingsFile]
    return [dict get $cfg cfg]
  }

  ## Create a variable in this namespace containing all the settings.
  set script_config [return_config_dict]
  ## Add the path to this script
  dict set script_config cfg scriptDir [file dirname [file normalize [info script]]]

}

## ----------------------------------------------------------------------------
## This proc creates the actual design content on the canvas. You can override 
## this with a custom procedure. Do this by creating your own version of this  
## procedure using the code below.
## 
## namespace eval ::roe::data {
##   proc design_build { ipRepo ipName ipCfgKey } {
##     ## Your design here
##   };# End of proc
## };# End of namespace
##
## ----------------------------------------------------------------------------
namespace eval ::roe::data {
  proc design_build { ipRepo ipName ipCfgKey board projectName } {
  
    ## add the framer and configure with settings passed in as arguments
    create_bd_cell -type ip -vlnv [dict get ${::roe::data::script_config} flow ipType] ${ipName}
    set_property -dict [::roe::bin::returnIpSettings $ipCfgKey] [get_bd_cells ${ipName}]
    
    ##  
    if { [get_property CONFIG.Production_Simulation [get_bd_cells ${ipName}] ] == 0x10000000} {

      puts "Build example design for HW. Factory use case."
      set ipRepo [::roe::bin::getIpRootDir]
      set version [get_property MODEL_NAME [get_ipdefs "*[dict get ${::roe::data::script_config} flow ipType]*"]]

      puts "Repo is $ipRepo using version ${version}."

      # Use the exdes flow
      #save_bd_design
      #open_example_project -in_process -force -dir ../output/${projectName}/vivado_exdes [get_ips] 
      
      # or Source the build scripts inline for speed
      source ${ipRepo}/tcl/helpers/ipi_common_ip_procs.tcl
      source ${ipRepo}/tcl/exdes/gen_torwave.tcl
      source ${ipRepo}/tcl/exdes/oran_monitor.tcl
      source ${ipRepo}/tcl/exdes/exdes_generate_procs.tcl

      ## Directly call the Exdes build Method
      if { $board == "vck190" } {
        ::xilinx.com::${version}::build_ipi_demo_design_inner ${ipName} 0 1 0
      } else {
        ::xilinx.com::${version}::build_ipi_demo_design_inner ${ipName} 1 1 0
      }

      set wrapperFile [make_wrapper -files [get_files *.bd -filter {NAME =~ *framer*}] -top]
      add_files $wrapperFile
      set_property top [get_bd_designs -filter {NAME =~ *framer*}]_wrapper [current_fileset]     

      if { $board == "zcu102" } {
        add_files -copy_to ../output/${projectName}/vivado/xdc -fileset constrs_1 -force -norecurse constraints/xorif_exd_zcu102.xdc
      }

      if { $board == "zcu111" } {
        add_files -copy_to ../output/${projectName}/vivado/xdc -fileset constrs_1 -force -norecurse constraints/xorif_exd_zcu111.xdc
      }

    } else {
    
      if { $ipRepo != "" } {
        puts "Manual source of BA scripts from IP repo. Factory use case."
        source ${ipRepo}/automation/auto_utils.tcl  
        ::xilinx.com_bd_rule_oran_radio_if::build_roe_demo_subsystem ${ipName} [::roe::bin::get_baSettings cust_repo_ptp]
      } else {
        puts "Call standard block automation"
        apply_bd_automation -rule xilinx.com:bd_rule:oran_radio_if -config [::roe::bin::get_baSettings norm_repo_ptp] [get_bd_cells ${ipName}]
      }
      
      add_qpll_reset ${ipName}
    
    }

  }
  
  proc add_qpll_reset { ipName } {
     
    create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice /datapath/framer_datapath/xlslice_0
    set_property -dict [list CONFIG.DIN_TO {1} CONFIG.DIN_FROM {1} CONFIG.DIN_WIDTH {8} CONFIG.DOUT_WIDTH {1}] [get_bd_cells datapath/framer_datapath/xlslice_0]
    
    connect_bd_net [get_bd_pins datapath/framer_datapath/xlslice_0/Din]  [get_bd_pins datapath/framer_datapath/${ipName}/user_rw_out]
    connect_bd_net [get_bd_pins datapath/framer_datapath/xlslice_0/Dout] [get_bd_pins datapath/xxv_eth_subs/xxv_wrap/xxv_ethernet_0/qpllreset_in_0]

  }
  
}

## Provide another hook to allow constraints, custom user modifications etc
## after the main design has been built.
namespace eval ::roe::data {
  
  proc puts_xorif { log_message } {
    puts "##----------------------------------------------------------------------"
    puts "## xorif_vivado_build: $log_message"
    puts "##----------------------------------------------------------------------"
  }

  proc run_user_mods { projectName board  mode ipRepo } {
    puts_xorif "Default run_user_mods, override in user_procs.tcl if you want to add custom logic."
  }

  proc design_modification { projectName board  mode ipRepo designType loop} {
  
    puts_xorif "Check for Design modifications"

    if { [get_property CONFIG.Production_Simulation [get_bd_cells -hierarchical roe_framer_0] ] == 0x10000000} {
      puts_xorif "No modifications in this test mode."
      run_user_mods $projectName $board  $mode $ipRepo $designType
      return
    }
  
    if { $board == "zcu111" } {
      puts_xorif "Modify REFCLK for zcu111"
      
      ## If the board is zcu111, ensure the REFCLK is 156.25MHz. The default BA
      ## can set this to 161 in 25G Mode, but we want to force this to the default
      ## rate of the Si570 on the board.
      set_property -dict [list CONFIG.GT_REF_CLK_FREQ {156.25}] [get_bd_cells datapath/xxv_eth_subs/xxv_wrap/xxv_ethernet_0]
      set_property CONFIG.FREQ_HZ 156250000                     [get_bd_intf_ports /gt_ref_clk]

      validate_bd_design
      set_property synth_checkpoint_mode None [get_files *.bd]
      generate_target all [get_files *.bd]

    }
    
    ## Check the rate, 10/25 and update all key parameters. If the internal_buc_clk rate is modified, these
    ## must be modified.
    set_property -dict [list CONFIG.OPTIMIZE_CLOCKING_STRUCTURE_EN {true} CONFIG.JITTER_SEL {Min_O_Jitter} CONFIG.CLKOUT1_DRIVES {BUFG} CONFIG.CLKOUT2_DRIVES {BUFG}] [get_bd_cells reset_retime/clk_wiz_syncE_timer]

    if { [get_property CONFIG.LINE_RATE [get_bd_cells /datapath/xxv_eth_subs/xxv_wrap/xxv_ethernet_0]] == 10} {
      puts_xorif "Modifications for Line Rate of 10gbps"
    
      #set_property -dict [list CONFIG.NO_OF_CLOCKS_FOR_1MS {156250}] [get_bd_cells datapath/framer_datapath/roe_radio_top_0] 
      #set_property -dict [list CONFIG.Xran_Timer_Clk_Ps    {6400}]   [get_bd_cells datapath/framer_datapath/roe_framer_0   ]

      ## Update the syncer clock period for 156.250MHz
      set_property -dict [list CONFIG.RESYNC_CLK_PERIOD {6400}]              [get_bd_cells datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/timer1588_subs/timer_sync_tx]
      set_property -dict [list CONFIG.RESYNC_CLK_PERIOD {6400}]              [get_bd_cells datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/timer1588_subs/timer_sync_rx]
      set_property -dict [list CONFIG.Xran_Timer_Clk_Ps {6400}]              [get_bd_cells datapath/framer_datapath/roe_framer_0]

      set_property -dict [list CONFIG.NO_OF_CLOCKS_FOR_1MS {156250}]         [get_bd_cells datapath/framer_datapath/roe_radio_top_0]
      set_property -dict [list CONFIG.clocks_for_1ms {156250}]               [get_bd_cells datapath/framer_datapath/oran_mon/radio_start_recover_v_0]
      set_property -dict [list CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {156.250} ] [get_bd_cells reset_retime/clk_wiz_syncE_timer]     


    } else {
      ## Update the syncer clock period for 390.625MHz
      set_property -dict [list CONFIG.RESYNC_CLK_PERIOD {2560}]              [get_bd_cells datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/timer1588_subs/timer_sync_tx]
      set_property -dict [list CONFIG.RESYNC_CLK_PERIOD {2560}]              [get_bd_cells datapath/xxv_eth_subs/xxv_wrap/support_1588_2step/timer1588_subs/timer_sync_rx]
      set_property -dict [list CONFIG.Xran_Timer_Clk_Ps {2560}]              [get_bd_cells datapath/framer_datapath/roe_framer_0]

      set_property -dict [list CONFIG.NO_OF_CLOCKS_FOR_1MS {390625}]         [get_bd_cells datapath/framer_datapath/roe_radio_top_0]
      set_property -dict [list CONFIG.clocks_for_1ms {390625}]               [get_bd_cells datapath/framer_datapath/oran_mon/radio_start_recover_v_0]
      set_property -dict [list CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {390.625} ] [get_bd_cells reset_retime/clk_wiz_syncE_timer]     
    }

    if {[regexp {om5} $mode] == 1} {
      puts_xorif "Add additional xdc constraint for ORAN mode."
      add_files -copy_to ../output/${projectName}/vivado/xdc -fileset constrs_1 -force -norecurse constraints/roe_framer_xdc_fifosync.xdc
    }

    if {$loop} {
      delete_bd_objs [get_bd_nets radio_start_10ms_stretch_0_1]
      connect_bd_net [get_bd_pins datapath/radio_start_10ms_stretch_0] [get_bd_ports timer_gen_10ms_toggle_0]
    }
    
    ##
    puts_xorif "Add toggle for debug"
    ## Use the following call to add the pin loc constraints
    add_toggle $board
    make_bd_pins_external  [get_bd_pins /datapath/framer_datapath/roe_framer_0/m0_dl_toggle]  

    puts_xorif "Add ILA"
    wa_add_additional_ila_of_interest
    
    run_user_mods $projectName $board  $mode $ipRepo $designType

    ## redo validation
    puts_xorif "design_modification done, re-validate"
    validate_bd_design

  }


  proc wa_add_core_reset_from_register_delete { } {
    ## This needs to hook on somewhere else  
    set ipName [get_bd_cells -hier -filter {VLNV =~ *:oran_radio_if:*}]

    create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 datapath/framer_datapath/xlslice_0
    connect_bd_net [get_bd_pins datapath/framer_datapath/xlslice_0/Din] [get_bd_pins ${ipName}/user_rw_out]
    set_property -dict [list CONFIG.DIN_TO {1} CONFIG.DIN_FROM {1} CONFIG.DIN_WIDTH {8} CONFIG.DOUT_WIDTH {1}] [get_bd_cells datapath/framer_datapath/xlslice_0]
    create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 datapath/framer_datapath/util_vector_logic_1
    set_property -dict [list CONFIG.C_SIZE {1} CONFIG.C_OPERATION {not} CONFIG.LOGO_FILE {data/sym_notgate.png}] [get_bd_cells datapath/framer_datapath/util_vector_logic_1]
    connect_bd_net [get_bd_pins datapath/framer_datapath/util_vector_logic_1/Op1] [get_bd_pins datapath/framer_datapath/xlslice_0/Dout]
    connect_bd_net [get_bd_pins datapath/framer_datapath/util_vector_logic_1/Res] [get_bd_pins datapath/framer_datapath/defm_reset_sync/aux_reset_in]

  }

  proc wa_add_additional_ila_of_interest { } {
      
    set ipName [get_bd_cells -hier -filter {VLNV =~ *:oran_radio_if:*}]
    
    ## Add the probes as interfaces
    set_property -dict [list CONFIG.C_NUM_OF_PROBES {3} CONFIG.C_NUM_MONITOR_SLOTS {4} CONFIG.C_MON_TYPE {MIX}] [get_bd_cells datapath/framer_datapath/oran_mon/ila_int]
    
    set_property -dict [list CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:xroe_display:xroe_dl_xran_header_if_rtl:1.0}] [get_bd_cells datapath/framer_datapath/oran_mon/ila_int]
    set_property -dict [list CONFIG.C_SLOT_3_INTF_TYPE {xilinx.com:xroe_display:xorif_timing_if_rtl:1.0}]        [get_bd_cells datapath/framer_datapath/oran_mon/ila_int]

    connect_bd_intf_net [get_bd_intf_pins ${ipName}/m0_message_axis] [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_int/SLOT_1_AXIS]                   
    connect_bd_intf_net [get_bd_intf_pins ${ipName}/m0_xran_head   ] [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_int/SLOT_2_XROE_DL_XRAN_HEADER_IF] 
    connect_bd_intf_net [get_bd_intf_pins ${ipName}/cc0_timing     ] [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_int/SLOT_3_XORIF_TIMING_IF] 

    connect_bd_net [get_bd_pins datapath/framer_datapath/oran_mon/radio_start_recover_v_0/radio_start_10ms]  [get_bd_pins datapath/framer_datapath/oran_mon/ila_int/probe0]
    connect_bd_net [get_bd_pins ${ipName}/m0_message_ts_tdata]                                               [get_bd_pins datapath/framer_datapath/oran_mon/ila_int/probe1]
    connect_bd_net [get_bd_pins ${ipName}/m0_message_ts_tvalid]                                              [get_bd_pins datapath/framer_datapath/oran_mon/ila_int/probe2]

    ## Add the One PPS
    set_property -dict [list CONFIG.C_NUM_OF_PROBES {4}] [get_bd_cells datapath/framer_datapath/oran_mon/ila_int]
    create_bd_cell -type module -reference roe_radio_ctrl_sync datapath/framer_datapath/oran_mon/roe_radio_ctrl_sync_0
    connect_bd_net [get_bd_pins datapath/framer_datapath/oran_mon/roe_radio_ctrl_sync_0/data_out] [get_bd_pins datapath/framer_datapath/oran_mon/ila_int/probe3]
    connect_bd_net [get_bd_pins datapath/framer_datapath/oran_mon/internal_bus_clk]               [get_bd_pins datapath/framer_datapath/oran_mon/roe_radio_ctrl_sync_0/clk]
    connect_bd_net [get_bd_pins datapath/framer_datapath/oran_mon/roe_radio_ctrl_sync_0/data_in]  [get_bd_pins datapath/xxv_eth_subs/one_pps_0]

    ## Update the ETH Probe
    set_property -dict [list CONFIG.C_NUM_OF_PROBES {3} CONFIG.C_MON_TYPE {MIX}] [get_bd_cells datapath/framer_datapath/oran_mon/ila_eth]
    
    connect_bd_net [get_bd_pins ${ipName}/s0_eth_mac_bad_fcs]      [get_bd_pins datapath/framer_datapath/oran_mon/ila_eth/probe0]
    connect_bd_net [get_bd_pins ${ipName}/s0_eth_mac_tstamp_out]   [get_bd_pins datapath/framer_datapath/oran_mon/ila_eth/probe1]
    connect_bd_net [get_bd_pins ${ipName}/s0_eth_mac_tstamp_valid] [get_bd_pins datapath/framer_datapath/oran_mon/ila_eth/probe2]

    if { [get_property CONFIG.Physical_Ethernet_Ports [get_bd_cells $ipName]] == 2 } {
    
      ## Add the AXIS Debug  
      set_property -dict [list CONFIG.C_NUM_MONITOR_SLOTS {2} ] [get_bd_cells datapath/framer_datapath/oran_mon/ila_int]
      connect_bd_intf_net [get_bd_intf_pins datapath/framer_datapath/roe_radio_top_0/sink_data_002] [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_int/SLOT_1_AXIS] 

      copy_bd_objs datapath/framer_datapath/oran_mon  [get_bd_cells {datapath/framer_datapath/oran_mon/ila_eth}]
      
      connect_bd_intf_net [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_eth1/SLOT_0_AXIS] [get_bd_intf_pins ${ipName}/s1_eth_axis]
      connect_bd_net [get_bd_pins datapath/framer_datapath/tx1_eth_port_clk] [get_bd_pins datapath/framer_datapath/oran_mon/ila_eth1/clk]
      
      delete_bd_objs [get_bd_intf_nets datapath/framer_datapath/oran_mon/ila_int_TRIG_OUT]
      connect_bd_intf_net [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_int/TRIG_OUT] [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_eth1/TRIG_IN]
      connect_bd_intf_net [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_eth/TRIG_IN] [get_bd_intf_pins datapath/framer_datapath/oran_mon/ila_eth1/TRIG_OUT]

      create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 datapath/framer_datapath/oran_mon/proc_sys_reset_0

      connect_bd_net [get_bd_pins datapath/framer_datapath/oran_mon/ila_eth1/resetn] [get_bd_pins datapath/framer_datapath/oran_mon/proc_sys_reset_0/interconnect_aresetn]
      connect_bd_net [get_bd_pins datapath/framer_datapath/oran_mon/tx1_eth_port_clk] [get_bd_pins datapath/framer_datapath/oran_mon/proc_sys_reset_0/slowest_sync_clk]
      connect_bd_net [get_bd_pins datapath/framer_datapath/oran_mon/resetn1] [get_bd_pins datapath/framer_datapath/oran_mon/proc_sys_reset_0/ext_reset_in]
    
    }
    
    ## Add reset on Framer
    create_bd_cell -type ip -vlnv xilinx.com:ip:vio datapath/framer_datapath/vio_0
    set_property -dict [list CONFIG.C_PROBE_OUT0_INIT_VAL {0x1} CONFIG.C_EN_PROBE_IN_ACTIVITY {0} CONFIG.C_NUM_PROBE_IN {0}] [get_bd_cells datapath/framer_datapath/vio_0]
    connect_bd_net [get_bd_pins datapath/framer_datapath/vio_0/probe_out0] [get_bd_pins datapath/framer_datapath/defm_reset_sync/aux_reset_in]
    connect_bd_net [get_bd_pins datapath/framer_datapath/m_axis_defm_aclk] [get_bd_pins datapath/framer_datapath/vio_0/clk]

  }
  
  ## ---------------------------------------------------------------------------
  ## Module that can be added to visualise pulse signals on external IO with OSC
  ## This port is now avaiable from the IP, but this is an example of how code
  ## can be added, via TCL, if required. It will then be avaiable in IPI via
  ## Module Referene Flow.
  ## ---------------------------------------------------------------------------
  proc add_toggle { board } {

    set fName "mrf_toggle.v"
    set string "
module mrf_toggle (
input      clk,
input      pulse_in,
output reg toggle_out=0 );

always @(posedge clk)
  toggle_out <= pulse_in ? ~toggle_out : toggle_out;
endmodule
"

    ## This is a neat way to add verilog modules
    #::xilinx.com::oran_radio_if_v1_1::writeStringToFile $fName $string
    #add_files -force -norecurse -copy_to [get_property DIRECTORY [current_project]] $fName
    ## from 20.2 IP has these avaiable as IO pins
    make_bd_pins_external  [get_bd_pins /datapath/framer_datapath/roe_framer_0/m0_dl_toggle]
    
    if { $board == "zcu111" } {
      set IO_TYPE "LVCMOS12"
      set IO_PIN  "J15"
    } else {
      set IO_TYPE "LVCMOS33"
      set IO_PIN  "J19"
    }  

    set fName "mrf_toggle.xdc"
    set string "
set_property PACKAGE_PIN $IO_PIN     \[get_ports \"m0_dl_toggle_0\"\]
set_property IOSTANDARD  $IO_TYPE    \[get_ports \"m0_dl_toggle_0\"\]
"

    #::xilinx.com::oran_radio_if_v1_1::writeStringToFile $fName $string
    set fileId [open $fName "w"]
    puts -nonewline $fileId $string
    close $fileId
    add_files -force -norecurse -fileset constrs_1 -copy_to [get_property DIRECTORY [current_project]] $fName

  }   

}

## ----------------------------------------------------------------------------
## 
## ----------------------------------------------------------------------------
namespace eval ::roe::bin {

  ## --------------------------------------------------------------------------
  ## Script Usage
  ## --------------------------------------------------------------------------
  proc usage { } {
  
    set mode   om5
    set ipRepo "[pwd]/path_to_repo"
    set board  zcu111
    set designType "exs"
    
    puts "
##-----------------------------------------------------------------------------
## Xilinx roe_framer Example System Starter Script.
## Use this to generate XSA for Petalinux.
## Extend to build your own System flows.
##-----------------------------------------------------------------------------
## Manually set variables
set mode   om5
set board  zcu111
set ipRepo \"\"
set doimpl     0
set exitOnDone 0

## Review the information in the configuration dict
::roe::bin::reload

## Review the information in the configuration dict
::roe::bin::showData

## Build command
::roe::bin::createIp_runIpiBuild <Optional IP Repo> <Build Mode(om0|om5)> <designType(exd|exs)> <Target Board(zcu102|zcu111)>

## Default
::roe::bin::createIp_runIpiBuild $ipRepo $mode $designType $board

## Once complete you can call implementation & then generate the XSA
::roe::bin::run_impl
::roe::bin::exportHw

##-----------------------------------------------------------------------------"
  }


  ## --------------------------------------------------------------------------
  ## Get the IP root from the component XML file.
  proc getIpRootDir {  } {
    
    set vlnv    [get_ipdefs  *:oran_radio_if:* -filter {UPGRADE_VERSIONS == ""} ]
    set xmlfile [get_property XML_FILE_NAME [get_ipdefs $vlnv]]
    regsub  {\/component.xml} $xmlfile "" xmlfile
    return "${xmlfile}"
    
  }

  ## --------------------------------------------------------------------------
  ## Create a variable in this namespace and assign the name of this script to it
  set fileName [file normalize [info script]]
  
  ## reload this script
  proc reload { } {
    source $::roe::bin::fileName
  }

  ## Dump all the script config data
  proc showData { } {
    foreach var [dict keys ${::roe::data::script_config} ] {
      puts "[format %-15s $var] : [dict get ${::roe::data::script_config} $var]"
    }
  }
  
  ## --------------------------------------------------------------------------
  ## Procs to pull nested values of from the configuration DICT with error 
  ## messaging should the values not exist.
  proc getL1DictSettings { {caller ""} {l1_key ""} {select ""} } {

    if { [dict exists ${::roe::data::script_config} $l1_key] } {
      if { [dict exists ${::roe::data::script_config} $l1_key $select] } {
        return [dict get ${::roe::data::script_config} $l1_key $select]
      } else {
        puts "Error $select does not exist for $caller"
        return ""
      }

    } else {
      puts "Error dict \[::roe::data::script_config $l1_key\] does not exist"
      return ""
    }

  }  

  proc getL2DictSettings { {caller ""} {l1_key ""} {l2_key ""} {select ""} } {

    if { [dict exists ${::roe::data::script_config} $l1_key] } {
      if { ! [dict exists ${::roe::data::script_config} $l1_key $l2_key] } {
        puts "Error L2 Key $l2_key has no settings"
        return "error $caller L2 Key"
      }
      if { ! [dict exists ${::roe::data::script_config} $l1_key $l2_key $select] } {
        puts "Error L2 Key $l2_key has no settings for Key $select"
        return "error $caller L2 Key Select"
      }
      return [dict get ${::roe::data::script_config} $l1_key $l2_key $select]
    } else {
      puts "Error dict \[::roe::data::script_config $l1_key\] does not exist"
    }
    return ""

  }  

  ## --------------------------------------------------------------------------
  ## Procs to pull values from the configuration dict. Fetch has a meaningful
  ## name and tests for existance each time.
  ## --------------------------------------------------------------------------
  proc returnIpSettings { {select ""} } {
    return [getL1DictSettings returnIpSettings ip $select]
  }  

  proc get_board { board name } {
    return [getL2DictSettings get_board boards $board $name]
  }
  
  proc get_baSettings { name } {
    return [getL1DictSettings get_baSettings bacfg $name]  
  }
  
  ## --------------------------------------------------------------------------
  ## Functions to build the design. This holds minimal Vivado TCL mechanics
  ## to get to a validated design and only bitstream and XSA.
  ## --------------------------------------------------------------------------
  proc create_projName { board mode designType noDateInProjName} {
    ## Create a uniqueish name based in the board, project name and Vivado
    ## version in use. The chosen name is last as this is likely to change/incrment
    ## during development runs. Therefore directory names should sort nicely
    ## on the users file browser.
    if { $noDateInProjName } {
      set pName "${board}_${mode}_${designType}_[version -short]"
    } else {
      set pName "${board}_${mode}_${designType}_[version -short]__[clock format [clock seconds] -format "%Y%m%d_%H%M%S"]"
    }
    ## Replace chars we dont want in the project name.
    regsub {\.+} $pName "_" pName
    regsub {\s+} $pName "_" pName
    puts "Creating project ${pName} & running Block Automation"
    return $pName
  }
  
  proc run_impl { } {
    ## Launch impl
    launch_runs impl_1 -to_step write_bitstream -jobs 4
    wait_on_run impl_1
    set helpString [exportHw]
    
    open_run impl_1
    report_utilization -hierarchical -hierarchical_depth 4 -hierarchical_percentages
    
    puts $helpString
    
    return impl_1
  }
  
  proc exportHw { } {

    ## Generate XSA/HDF for Petalinux
    set  dir     [get_property DIRECTORY [current_project]]
    set  name    [get_property NAME [current_project]]
    #set  sdkPath ${dir}/${name}.sdk
    #file mkdir   ${sdkPath}
  
    ## Just write at the top level so its easier to find.
    write_hw_platform -fixed -force -include_bit -file ${dir}/system.xsa
      
    ## Give the user some help
    puts "Target Modification 1 : $name"
    regsub "_exs_.+" $name "_exs" target
    puts "Target Modification 2 : $target"
    regsub "_exd_.+" $target "_exd" target
    puts "Target Modification 3 : $target"
    return " 
################################################################################
## Possible command sequence to launch Petalinux
################################################################################
source /proj/petalinux/2021.2/petalinux-v2021.2_daily_latest/tool/petalinux-v2021.2-final/settings.csh
mkdir ../xsa/$target
cp ${dir}/system.xsa ../xsa/${target}/system.xsa   
make $target
################################################################################
## $target
################################################################################
"

  }

  ## Select the part/board we want, create a project anme and the project
  proc configure_project { ipRepo mode board designType noDateInProjName } {
    ## 
    set PART  [get_board $board PART]
    set BOARD [get_board $board BOARD]
    set pName [create_projName $board $mode $designType $noDateInProjName]
  
	  create_project ${pName} ../output/${pName}/vivado -part $PART -force
    set_property board_part $BOARD [current_project]
  
    if { $ipRepo != "" } {
      ## Add a custom IP repo
      set_property  ip_repo_paths  ${ipRepo} [current_project]
      update_ip_catalog
    }

    return $pName
  
  }
  
  ## Create project/Board Design/Example design and validate
  proc createIp_runIpiBuild { ipRepo mode board designType noDateInProjName } {

    set ipName roe_framer_0
    set pName [configure_project $ipRepo $mode $board $designType $noDateInProjName]

    create_bd_design "${ipName}_exdes_ipi"
  
    ## Call the design script, separate proc so it can be loaded seperately.
    ::roe::data::design_build $ipRepo $ipName $mode $board $pName
  
    ## Validate and write BD tcl
    validate_bd_design
    write_bd_tcl -force -no_ip_version -exclude_layout ${pName}.tcl
    
    return $pName
  
  }

  ## Pretty print the timing stats post build.
  proc reportImplStatus {runName} {
     
     ## Get some useful stats on tun
     set STATS_TNS [get_property STATS.TNS [get_runs $runName]]
     set STATS_WNS [get_property STATS.WNS [get_runs $runName]]
     set DIRECTORY [get_property DIRECTORY [get_runs $runName]]
  
     foreach stat [list_property [get_runs $runName]] { if {[regexp STATS\..+ $stat]} { 
       puts "      -> [format %-40s $stat] [get_property $stat [get_runs $runName]]" }
     }
  
     puts "\nOutput generated in            : $DIRECTORY"
  
     if { $STATS_TNS < 0 } {
        puts "Error: timing FAILED on run $runName TNS: $STATS_TNS WNS: $STATS_WNS\n"
        return 1
     } else {
        puts "Timing PASSED on run $runName TNS: $STATS_TNS WNS: $STATS_WNS\n"
        return 0
     }
  
  }

}

## ----------------------------------------------------------------------------
## Example of how you can use the -tclargs switch in Vivado to selct the build
## you want to run. Alternatively, you can just source this script and use
## function calls provided by it.
## ----------------------------------------------------------------------------
## Process the command line arguments
proc process_tclargs { argc argv } {
  if {$argc > 0} {
  
    ## Flow control variables
    set mode             om5
    set board            zcu102
    ## Use the repo in the build
    set ipRepo           ""
    set designType       "exs"
    set doimpl           0
    set exitOnDone       0
    set startGui         0
    set noDateInProjName 0
    set loop             0
  
    ## This style uses a set of strings concatenated together to tell the 
    ## script what mode it should build in
    ## 1st TCLARGS = Board to target
    ## 2nd TCLARGS = IP Mode
    ## 3rd TCLARGS = Command mode to run
    ## 4rd TCLARGS = IP Directory
  
    if {$argc > 0} {
      set argIn  [string tolower [lindex $argv 0]]
      puts "Using tclargs lowercased to $argIn - Using as board used"
      set board $argIn
    }
  
    if {$argc > 1} {
      set argIn  [string tolower [lindex $argv 1]]
      puts "Using tclargs lowercased to $argIn - Using as ip config mode"
      set mode $argIn
    }

    if {$argc > 2} {
      set argIn  [string tolower [lindex $argv 2]]
      puts "Using tclargs lowercased to $argIn"
      if { [regexp {exd}    $argIn] == 1}     { set designType "exd" }
      if { [regexp {gui}    $argIn] == 1}     { set startGui   1     }
      if { [regexp {loop}   $argIn] == 1}     { set loop       1     }
      if { [regexp {impl}   $argIn] == 1}     { set doimpl     1     }
      if { [regexp {exit}   $argIn] == 1}     { set exitOnDone 1     }
      if { [regexp {nodate} $argIn] == 1}     { set noDateInProjName 1   }
    }
  
    if {$argc > 3} {
      set argIn  [lindex $argv 3]
      puts "Using $argIn as IP Repo"
      set ipRepo $argIn
    }
  
    puts "**********************************************************************"
    puts "Launching\n::roe::bin::createIp_runIpiBuild $ipRepo $mode $board $designType"
    puts "**********************************************************************"
    set projectName [::roe::bin::createIp_runIpiBuild $ipRepo $mode $board $designType $noDateInProjName]

    puts "roePROJECTNAME: $projectName"

    ## Add board specific constraints in automation.
    ::roe::data::design_modification $projectName $board $mode $ipRepo $designType $loop
  
    foreach prop [list_property [get_bd_cells -hier -filter {VLNV =~ *oran_radio_if*}]] { 
      puts "[format %-40s $prop] [get_property $prop [get_bd_cells -hier -filter {VLNV =~ *oran_radio_if*}]]"
    }
  
    ## Run the implementation
    if { $doimpl == 1 } {
    
      puts "**********************************************************************"
      puts "Launching Implementation, this will take about an hour, with no log to screen."
      set runName [::roe::bin::run_impl]
      puts "**********************************************************************"
      ::roe::bin::reportImplStatus $runName
      puts "**********************************************************************"
    } 

    if { $startGui } {
      start_gui
    }
  
    if { $exitOnDone } {
      exit
    }
  
  }
} 

## ----------------------------------------------------------------------------
## Source a custom user file, this allows over-riding of any of the procs
## defined so far.
## ----------------------------------------------------------------------------
set USER_TCL_FILE "user_procs.tcl"
if { [file exists $USER_TCL_FILE] } {
  puts "You want to do someting cool, sourcing your stuff in $USER_TCL_FILE"  
  source $USER_TCL_FILE
}

## ----------------------------------------------------------------------------
## Process the command line arguments, show the help text
## ----------------------------------------------------------------------------
if { [info exists argc] } {
  ::roe::bin::usage
  process_tclargs $argc $argv
  ::roe::bin::usage
} else {
  ::roe::bin::usage
}
