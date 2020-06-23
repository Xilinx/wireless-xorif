
# 25G ETHERNET INTERFACE CONNECTION

## SFP Enable
set_property PACKAGE_PIN G12      [get_ports {sfp_enable[0]}]
set_property IOSTANDARD HSTL_I_12 [get_ports {sfp_enable[0]}]

#############
# LED 
#############
set_property PACKAGE_PIN AV15 [get_ports {LED_tri_o[7]}]
set_property PACKAGE_PIN AN17 [get_ports {LED_tri_o[6]}]
set_property PACKAGE_PIN AN16 [get_ports {LED_tri_o[5]}]
set_property PACKAGE_PIN AP15 [get_ports {LED_tri_o[4]}]
set_property PACKAGE_PIN AP16 [get_ports {LED_tri_o[3]}]
set_property PACKAGE_PIN AR16 [get_ports {LED_tri_o[2]}]
set_property PACKAGE_PIN AP13 [get_ports {LED_tri_o[1]}]
set_property PACKAGE_PIN AR13 [get_ports {LED_tri_o[0]}]


set_property IOSTANDARD LVCMOS18 [get_ports {LED_tri_o[7]}]
set_property IOSTANDARD LVCMOS18 [get_ports {LED_tri_o[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {LED_tri_o[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {LED_tri_o[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {LED_tri_o[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {LED_tri_o[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {LED_tri_o[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {LED_tri_o[0]}]

#############
# DIP SWITCHES
#############
set_property PACKAGE_PIN AF16    [get_ports {gpio_cdc_dipstatus[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_cdc_dipstatus[0]}]
set_property PACKAGE_PIN AF17    [get_ports {gpio_cdc_dipstatus[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_cdc_dipstatus[1]}]
set_property PACKAGE_PIN AH15    [get_ports {gpio_cdc_dipstatus[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_cdc_dipstatus[2]}]
set_property PACKAGE_PIN AH16    [get_ports {gpio_cdc_dipstatus[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_cdc_dipstatus[3]}]
set_property PACKAGE_PIN AH17    [get_ports {gpio_cdc_dipstatus[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_cdc_dipstatus[4]}]
set_property PACKAGE_PIN AG17    [get_ports {gpio_cdc_dipstatus[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_cdc_dipstatus[5]}]
set_property PACKAGE_PIN AJ15    [get_ports {gpio_cdc_dipstatus[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_cdc_dipstatus[6]}]
set_property PACKAGE_PIN AJ16    [get_ports {gpio_cdc_dipstatus[7]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_cdc_dipstatus[7]}]

#############
# Clocks
#############

# 300 MHz Reference Clock  ( BUT NEED TO PROGRAM DEFAULT 200MHz/100MHz OSCILLATOR OR CHANGE MMCM CONFIG !!! )
# AM15 Default freq is 100MHz CLOCK INPUT ( BUT NEED TO PROGRAM DEFAULT 100MHz OSCILLATOR ORCHANGE THE MMCM CONFIG !!! )
set_property PACKAGE_PIN AM15 [get_ports clk_300m_0_clk_p]
set_property PACKAGE_PIN AN15 [get_ports clk_300m_0_clk_n]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports clk_300m_0_clk_n]

#############
# QSFP Clocks. This comes from the Quad below the Quad used. USER_MGT_SI570
#############
set_property PACKAGE_PIN V32 [get_ports gt_ref_clk_clk_n]
set_property PACKAGE_PIN V31 [get_ports gt_ref_clk_clk_p]

#############
# QSFP0 ports
#############

## PORT 0
## quad _128 FOR CONNECTION BELOW
## 
set_property PACKAGE_PIN AA39 [get_ports gt_serial_port_0_grx_n]
set_property PACKAGE_PIN AA38 [get_ports gt_serial_port_0_grx_p]
set_property PACKAGE_PIN Y36  [get_ports gt_serial_port_0_gtx_n]
set_property PACKAGE_PIN Y35  [get_ports gt_serial_port_0_gtx_p]


## Clock constraints
#390.625MHz for 25G ethernet
#create_clock -period 2.560 -name clk_refclk -waveform {0.000 1.280} [get_ports gt_ref_clk_clk_n]
#create_clock -period 2.560 -name clk_refclk -waveform {0.000 1.280} [get_ports gt_ref_clk_clk_p]

##-----------------------------------------------------------------------------
## Use center pushbutton as reset
##-----------------------------------------------------------------------------
set_property PACKAGE_PIN AF15        [get_ports "pushbutton_reset"]
set_property IOSTANDARD  LVCMOS18    [get_ports "pushbutton_reset"]

## ONE PPS placement pin
set_property PACKAGE_PIN L14         [get_ports {one_pps_0}]
set_property IOSTANDARD  LVCMOS12    [get_ports {one_pps_0}]


set_property PACKAGE_PIN V31 [get_ports si570_in_n ]
set_property PACKAGE_PIN V32 [get_ports si570_in_p ]
set_property PACKAGE_PIN Y31 [get_ports si570_out_p]
set_property PACKAGE_PIN Y32 [get_ports si570_out_n]
