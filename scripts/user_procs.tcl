
namespace eval ::roe::data {

  proc run_user_mods { projectName board  mode ipRepo designType } {
    puts "Running run_user_mods proc:"
    puts "    projectName $projectName"
    puts "    board       $board"
    puts "    mode        $mode"
    puts "    ipRepo      $ipRepo"
    puts "    designType  $designType"

    if { $designType eq "exs" } {
      puts "This design is the Example System. It is recomended not to use this file for the main system, only for the Example Design, EXD. Returning immediately."
      return
    }

    puts "USER: add probes & other connections"
    add_zcu_exd_connections $board
    

    if { [llength [get_bd_cells oran_mon/ila_int]] > 0 } {

      #set ocp_in_ip [llength [list_property [get_bd_cells -hier -filter { VLNV =~ *oran_radio_if*}] CONFIG.Use_OCP_In_IP]]

      #if {$ocp_in_ip} {
      #  set_property -dict [list CONFIG.C_NUM_MONITOR_SLOTS {3}] [get_bd_cells oran_mon/ila_int]
      #} else {
        set_property -dict [list CONFIG.C_NUM_OF_PROBES {1} CONFIG.C_MON_TYPE {MIX} CONFIG.C_NUM_MONITOR_SLOTS {3}] [get_bd_cells oran_mon/ila_int]
        connect_bd_net [get_bd_pins /torwave_0/radio_offset_10ms_stretch] [get_bd_pins oran_mon/ila_int/probe0]
      #}

      set ipName roe_framer_0

      set_property -dict [list CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:xroe_display:xroe_dl_xran_header_if_rtl:1.0}] [get_bd_cells oran_mon/ila_int]
      set_property -dict [list CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:xroe_display:xorif_timing_if_rtl:1.0}]        [get_bd_cells oran_mon/ila_int]

      connect_bd_intf_net [get_bd_intf_pins ${ipName}/m0_xran_head   ] [get_bd_intf_pins oran_mon/ila_int/SLOT_1_XROE_DL_XRAN_HEADER_IF] 
      connect_bd_intf_net [get_bd_intf_pins ${ipName}/cc0_timing     ] [get_bd_intf_pins oran_mon/ila_int/SLOT_2_XORIF_TIMING_IF] 

      set_property -dict [ list \
       CONFIG.C_NUM_MONITOR_SLOTS {2} \
       CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
      ] [get_bd_cells oran_mon/ila_eth]

      connect_bd_intf_net [get_bd_intf_pins oran_mon/ila_eth/SLOT_1_AXIS] [get_bd_intf_pins  ${ipName}/m0_eth_axis]

      ## Shrink this as device is pretty full! 96% BRAM
      ## If deeper ILA needed the XRAN_HEADER can be dropped and selective probes added
      set_property -dict [list CONFIG.C_DATA_DEPTH {4096}] [get_bd_cells oran_mon/ila_int]
      set_property -dict [list CONFIG.C_DATA_DEPTH {4096}] [get_bd_cells oran_mon/ila_eth]
    
      set ocp_in_ip_version [llength [list_property [get_bd_cells -hier -filter { VLNV =~ *oran_radio_if*}] CONFIG.Use_OCP_In_IP]]

      if {$ocp_in_ip_version} {
        set ocp_in_ip [get_property  CONFIG.Use_OCP_In_IP [get_bd_cells -hier -filter { VLNV =~ *oran_radio_if*}]]
      
        if {$ocp_in_ip} {
          set ila_ft [ create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila oran_mon/ila_ft ]
          set_property -dict [ list \
           CONFIG.C_DATA_DEPTH {4096} \
           CONFIG.C_NUM_MONITOR_SLOTS {1} \
           CONFIG.C_SLOT_0_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
           CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
           CONFIG.C_TRIGIN_EN {true} \
           CONFIG.C_TRIGOUT_EN {true} \
          ] $ila_ft
          #set_property -dict [list CONFIG.C_NUM_OF_PROBES {1} CONFIG.C_MON_TYPE {MIX} CONFIG.C_NUM_MONITOR_SLOTS {3}] [get_bd_cells oran_mon/ila_int]
          connect_bd_net  [get_bd_pins /clk_wiz_ft_aclk/clk_out1] [get_bd_pins $ila_ft/clk]
          connect_bd_net  [get_bd_pins /clk_wiz_ft_aclk/locked]   [get_bd_pins $ila_ft/resetn]

          connect_bd_intf_net [get_bd_intf_pins  $ila_ft/SLOT_0_AXIS]      [get_bd_intf_pins $ipName/m_dl_ft]
       
          delete_bd_objs      [get_bd_intf_nets oran_mon/ila_eth_TRIG_OUT]
          connect_bd_intf_net [get_bd_intf_pins oran_mon/ila_eth/TRIG_OUT] [get_bd_intf_pins  $ila_ft/TRIG_IN]
          connect_bd_intf_net [get_bd_intf_pins  $ila_ft/TRIG_OUT]         [get_bd_intf_pins oran_mon/ila_int/TRIG_IN]
       
        }
      }
    
    } else {
      puts "USER: oran_mon block was not found!"  
    }
    
    ## re validate
    validate_bd_design

  }

  proc add_zcu_exd_connections { board } {

    ## Will need to add some board selection here
    ## This can now be done in the core YAML
    #set_property -dict [list CONFIG.memory_word_depth {32768}] [get_bd_cells torwave_0]
    #set_property -dict [list CONFIG.memory_word_depth {65536}] [get_bd_cells torwave_0]
    #set_property -dict [list CONFIG.memory_word_depth {135168}] [get_bd_cells torwave_0]
    #set_property -dict [list CONFIG.memory_word_depth {122880}] [get_bd_cells torwave_0]

    if { $board eq "vck190" } {

      return 
      
    }

    create_bd_port -dir O radio_offset_10ms_stretch
    connect_bd_net [get_bd_pins /torwave_0/radio_offset_10ms_stretch] [get_bd_ports radio_offset_10ms_stretch]
    create_bd_port -dir O radio_start_10ms_stretch
    connect_bd_net [get_bd_pins /torwave_0/radio_start_10ms_stretch]  [get_bd_ports radio_start_10ms_stretch]

    if { [get_property CONFIG.Physical_Ethernet_Rate [get_bd_cells /roe_framer_0]] == "10G"} {

      set_property -dict [list CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {200.000} CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {200.000} CONFIG.MMCM_CLKOUT1_DIVIDE {6} CONFIG.MMCM_CLKOUT2_DIVIDE {6} CONFIG.CLKOUT2_JITTER {102.086} CONFIG.CLKOUT3_JITTER {102.086}] [get_bd_cells clk_wiz_0]
      set_property -dict [list CONFIG.Xran_Timer_Clk_Ps {5000}] [get_bd_cells roe_framer_0]
      set_property -dict [list CONFIG.clocks_for_10ms {2000000} CONFIG.clocks_for_1ms {200000} CONFIG.ps_per_clock {5000}] [get_bd_cells torwave_0]

    } else {

      set_property -dict [list CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {400.000} CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {400.000}] [get_bd_cells clk_wiz_0]
      set_property -dict [list CONFIG.Xran_Timer_Clk_Ps {2500}] [get_bd_cells roe_framer_0]
      set_property -dict [list CONFIG.clocks_for_10ms {4000000} CONFIG.clocks_for_1ms {400000} CONFIG.ps_per_clock {2500}] [get_bd_cells torwave_0]

    
    }
    set fName "zcu_exd_connections.xdc"
    
    if { $board eq "zcu111" } {
    
      set string "
## IO for strobing
set_property PACKAGE_PIN M18        \[get_ports {radio_offset_10ms_stretch}\]
set_property IOSTANDARD  LVCMOS12   \[get_ports {radio_offset_10ms_stretch}\]
set_property PACKAGE_PIN J15        \[get_ports {radio_start_10ms_stretch} \]
set_property IOSTANDARD  LVCMOS12   \[get_ports {radio_start_10ms_stretch} \]
"
    } else {
    
      set string "
set_property PACKAGE_PIN E20        \[get_ports {radio_offset_10ms_stretch}\]
set_property IOSTANDARD  LVCMOS33   \[get_ports {radio_offset_10ms_stretch}\]
set_property PACKAGE_PIN J19        \[get_ports {radio_start_10ms_stretch} \]
set_property IOSTANDARD  LVCMOS33   \[get_ports {radio_start_10ms_stretch} \]
"

      set_property -dict [list CONFIG.PSU__UART0__PERIPHERAL__IO {MIO 18 .. 19}] [get_bd_cells app_cpu]
      delete_bd_objs [get_bd_ports emio_uart0_txd_0]
      delete_bd_objs [get_bd_ports emio_uart0_rxd_0]

    }

    #::xilinx.com::oran_radio_if_v2_0::writeStringToFile $fName $string
    set fileId [open $fName "w"]
    puts -nonewline $fileId $string
    close $fileId

    add_files -force -norecurse -fileset constrs_1 -copy_to [get_property DIRECTORY [current_project]] $fName

  }

}
