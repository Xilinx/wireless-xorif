
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
    
    set_property -dict [list CONFIG.C_NUM_OF_PROBES {12} CONFIG.C_MON_TYPE {MIX}] [get_bd_cells oran_mon/ila_int]
    
    set ipName roe_framer_0
    
    connect_bd_net [get_bd_pins ${ipName}/m0_dl_cta_sym_num]          [get_bd_pins oran_mon/ila_int/probe0]
    connect_bd_net [get_bd_pins ${ipName}/m0_dl_sym_num]              [get_bd_pins oran_mon/ila_int/probe1]
    connect_bd_net [get_bd_pins ${ipName}/m0_dl_update]               [get_bd_pins oran_mon/ila_int/probe2]
    connect_bd_net [get_bd_pins ${ipName}/m0_ul_cta_sym_num]          [get_bd_pins oran_mon/ila_int/probe3]
    connect_bd_net [get_bd_pins ${ipName}/m0_ul_sym_num]              [get_bd_pins oran_mon/ila_int/probe4]
    connect_bd_net [get_bd_pins ${ipName}/m0_ul_update]               [get_bd_pins oran_mon/ila_int/probe5]
    connect_bd_net [get_bd_pins ${ipName}/m0_radio_app_head_valid]    [get_bd_pins oran_mon/ila_int/probe6]
    connect_bd_net [get_bd_pins ${ipName}/m0_section_header_valid]    [get_bd_pins oran_mon/ila_int/probe7]
    connect_bd_net [get_bd_pins ${ipName}/m0_t_header_offset_valid]   [get_bd_pins oran_mon/ila_int/probe8]
    connect_bd_net [get_bd_pins ${ipName}/m0_packet_in_window]        [get_bd_pins oran_mon/ila_int/probe9]
    connect_bd_net [get_bd_pins /torwave_0/radio_offset_10ms_stretch] [get_bd_pins oran_mon/ila_int/probe10]
    connect_bd_net [get_bd_pins ${ipName}/m0_offset_in_symbol]        [get_bd_pins oran_mon/ila_int/probe11]

    ## re validate
    validate_bd_design

  }

  proc add_zcu_exd_connections { board } {
    delete_bd_objs [get_bd_nets roe_radio_top_0_fram_radio_start_10ms]
    delete_bd_objs [get_bd_nets roe_radio_top_0_defm_radio_start_10ms]
    connect_bd_net [get_bd_pins roe_framer_0/defm_radio_start_10ms]    [get_bd_pins torwave_0/radio_offset_10ms]
    connect_bd_net [get_bd_pins roe_framer_0/fram_radio_start_10ms]    [get_bd_pins torwave_0/radio_offset_10ms]

    ## Will need to add some board selection here
    #set_property -dict [list CONFIG.memory_word_depth {32768}] [get_bd_cells torwave_0]
    #set_property -dict [list CONFIG.memory_word_depth {65536}] [get_bd_cells torwave_0]
    #set_property -dict [list CONFIG.memory_word_depth {135168}] [get_bd_cells torwave_0]
    set_property -dict [list CONFIG.memory_word_depth {122880}] [get_bd_cells torwave_0]

    create_bd_port -dir O radio_offset_10ms_stretch
    connect_bd_net [get_bd_pins /torwave_0/radio_offset_10ms_stretch] [get_bd_ports radio_offset_10ms_stretch]
    create_bd_port -dir O radio_start_10ms_stretch
    connect_bd_net [get_bd_pins /torwave_0/radio_start_10ms_stretch]  [get_bd_ports radio_start_10ms_stretch]

    set_property -dict [list CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {200.000} CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {200.000} CONFIG.MMCM_CLKOUT1_DIVIDE {6} CONFIG.MMCM_CLKOUT2_DIVIDE {6} CONFIG.CLKOUT2_JITTER {102.086} CONFIG.CLKOUT3_JITTER {102.086}] [get_bd_cells clk_wiz_0]
    set_property -dict [list CONFIG.Xran_Timer_Clk_Ps {5000}] [get_bd_cells roe_framer_0]
    set_property -dict [list CONFIG.clocks_for_10ms {2000000} CONFIG.clocks_for_1ms {200000} CONFIG.ps_per_clock {5000}] [get_bd_cells torwave_0]

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

    ::xilinx.com::oran_radio_if_v1_1::writeStringToFile $fName $string
    add_files -force -norecurse -fileset constrs_1 -copy_to [get_property DIRECTORY [current_project]] $fName

  }

}
