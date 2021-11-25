# Change Log

## Release 2021.2
* Updated register map
* Added RU port mapping table: xorif_set_ru_ports_table_mode(), xorif_clear_ru_ports_table(), xorif_set_ru_ports_table()
* xorif_set_system_constants() API now uses 'struct' (currently only has FH_DECAP_DLY)
* xorif_set_cc_time_advance() and xorif_set_ul_bid_forward() APIs now take doubles rather than uint32
* xorif_set_ul_radio_ch_dly() API added to allow setting of UL_RADIO_CH_DLY per component carrier
* Improved debug logging
* Improved code coverage testing
* Change interrupt handler so call-back called 'then' default processing
* xorif_set_cc_time_advance() API deprecated in favour of xorif_set_cc_ul_timing_parameters() and xorif_set_cc_dl_timing_parameters()
* xorif_set_ul_radio_ch_dly() API deprecated in favour of xorif_set_cc_ul_timing_parameters()
* xorif_set_cc_dl_sections_per_symbol(), xorif_set_cc_ul_sections_per_symbol() and xorif_set_cc_sections_per_symbol_ssb() APIs now takes parameters of "number_of_sections" and "number_of_ctrl_words"
* Fixed calculation for number of symbols (UL Ctrl, DL Ctrl and DL Data)
* Fixed calculation for register programming SETUP_C_ABS_SYMBOL and SETUP_C_CYCLES for uplink, downlink and UL BID forward

## Release 2021.1
* Refactored library code
* Changes for code coverage tools
* Updated register map
* Additional capabilities added to xorif_caps
* PRACH compression configuration added to xorif_cc_config
* Uplink beam-id forward time advance added to xorif_cc_config
* Buffer allocation procedure made more flexible
* xorif_init() API modified (beamformer device removed)
* xorif_get_enabled_cc_mask() API added
* xorif_set_ul_bid_forward() API added (uplink beam-id forward time advance)
* xorif_set_cc_dl_iq_compression() API modified (now takes static/dynamic flag)
* xorif_set_cc_ul_iq_compression() API modified (now takes static/dynamic flag)
* xorif_set_cc_iq_compression_ssb() API modified (now takes static/dynamic flag)
* xorif_set_cc_iq_compression_prach() API added
* xorif_get_fhi_hw_internal_rev() API modified (name changed)
* xorif_set_fhi_vlan_tag() API modified (takes separate 'ID', 'DEI' and 'PCP' fields)
* xorif_set_ru_ports_alt1() API added (c.f. xorif_set_ru_ports() but has LTE option)
* xorif_set_timing_constants() API added (allows user to set FH_DECAP_DLY, UL_RADIO_CH_DLY values)