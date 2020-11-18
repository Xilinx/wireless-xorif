
## Internal register paths
set_false_path -from [get_pins -hier -regexp {.*torwave_src_i\/djictl_.*\/(?:C)}]                                   -to [get_pins -hier -regexp {.*torwave_src_i\/slv_rdata_reg.*(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*torwave_regif_axi4l_regif_i\/(?:slv_addr_reg).*\/C}]                -to [get_pins -hier -regexp {.*torwave_src_i\/djictl_.*\/(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*torwave_src_i\/(?:slv_rden_r_reg|slv_wdata_r_internal_reg).*\/C}]   -to [get_pins -hier -regexp {.*torwave_src_i\/djictl_.*\/(?:CE|D|R)}]

## False path the RO registers. These are sampled, therefore stable when read
set_false_path -from [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/.*sink\/djictl_.*\/(?:C)}]                          -to [get_pins -hier -regexp {.*torwave_snk_i\/slv_rdata_reg.*(?:CE|D|R)}]

set_false_path -from [get_pins -hier -regexp {.*torwave_snk_i\/djictl_.*\/(?:C)}]                                   -to [get_pins -hier -regexp {.*torwave_snk_i\/slv_rdata_reg.*(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*torwave_regif_axi4l_regif_i\/(?:slv_addr_reg).*\/C}]                -to [get_pins -hier -regexp {.*torwave_snk_i\/djictl_.*\/(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*torwave_snk_i\/(?:slv_rden_r_reg|slv_wdata_r_internal_reg).*\/C}]   -to [get_pins -hier -regexp {.*torwave_snk_i\/djictl_.*\/(?:CE|D|R)}]

## Torwave data generator paths
set_false_path -from [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/sel_dij_data_rd.*\/(?:C)}]                          -to [get_pins -hier -regexp {.*torwave_regif_axi4l_regif_i\/(?:axi_rdata_reg).*\/(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/sel_dij_address_read.*\/(?:C)}]                     -to [get_pins -hier -regexp {.*torwave_regif_axi4l_regif_i\/(?:axi_rdata_reg).*\/(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/eth_src.+dg\/dij_data_rd_reg.*\/(?:C)}]             -to [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/sel_dij_data_rd_reg.*\/(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/eth_src.+dg\/dij_address_read.*\/(?:C)}]            -to [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/sel_dij_address_read.*\/(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/reg_if\/torwave_cfg_i\/dij_bank_select.*\/(?:C)}]   -to [get_pins -hier -regexp {.*tw_v2sv\/tw_sv\/(?:sel_dij_address|sel_dij_data).*\/(?:CE|D|R)}]

## Internal register paths
set_false_path -from [get_pins -hier -regexp {.*sspuc_ti_sink_i\/sspuc_snk_.*\/(?:C)}]                              -to [get_pins -hier -regexp {.*sspuc_ti_sink_i\/slv_rdata_reg.*(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*\/sspuc_ti_ctrl_axi4l_regif_i\/(?:slv_addr_reg).*\/C}]              -to [get_pins -hier -regexp {.*sspuc_ti_sink_i\/sspuc_snk_.*\/(?:CE|D|R)}]
set_false_path -from [get_pins -hier -regexp {.*sspuc_ti_sink_i\/(?:slv_rden_r_reg|slv_wdata_r_internal_reg).*\/C}] -to [get_pins -hier -regexp {.*sspuc_ti_sink_i\/sspuc_snk_.*\/(?:CE|D|R)}]

## False path the RO registers. These are sampled, therefore stable when read
set_false_path -from [get_pins -hier -regexp {.*\/v2sv\/sv\/sink.+\/snk_stat_.*\/(?:C)}]                            -to [get_pins -hier -regexp {.*\/v2sv\/sv\/bank\/sspuc_ti_sink_i\/slv_rdata_reg.*(?:CE|D|R)}]
