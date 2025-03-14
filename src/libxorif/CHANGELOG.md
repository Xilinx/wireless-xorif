# Change Log

## Release 2024.2-fix1
* Fixed bug in downlink timing offset calculation (CR-1229486).

## Release 2024.2
* No significant changes

## Release 2024.1
* Updated register map
* Added support for "UL Section Sort" feature

## Release 2023.2
* Change order of operations during xorif_reset_fhi()
* Function xorif_clear_ru_ports_table() sets everything to "all ones" (type = UNKNOWN)
* Added multiplier feature when programming OCP inter-symbol-gap value
* Added API for stall detection logic

## Release 2023.1
* Added "example1.py" script to test basic configuration of 1 CC (100MHz/30kHz) via Python/Pyro
* Integrated O-RAN Channel Process block into IP (merged SW driver, separate APIs)
* Updated development build / tests / code coverage / linting
* Release 2023.1 has ORIF integrated with OCP, which uses common s/w driver with separate APIs
* Added xorif_set_ru_ports_table_vcc() which is alternate API for RU port table "mode 2" (uses "Virtual CCID")
* Added xorif_has_oran_channel_processor() to check for presence of OCP
* Changed name of API function xorif_set_ru_ports_alt1() to xorif_set_ru_ports_lte()
* Updated register map

## Release 2022.2
* Added Python bindings for C library (libxorif)
* Add xorif_set_modu_mode() and xorif_set_modu_dest_mac_addr() for multi O-DU MAC destination address / VLAN tags
* Added "monitor" API: xorif_monitor_clear(), xorif_monitor_select(), xorif_monitor_snapshot(), xorif_monitor_read()
* Added xorif_set_cc_num_rbs_ssb() to allow SSB RBs to be set to 0 (no SSB)
* Support for 9-bit uncompressed mode in DL
* Test scripts added (were previously in separate internal directory)
* Fixed in-core/external compression capabilities

## Release 2022.1
* Updated register map
* Modified Yocto recipes for 'honister' release
* Makefile improvements
* Split doxygen-generated documentation into separate configs
* Added xorif_set_fhi_protocol_alt() API which just sets protocol, and not the packet filter configuration
* Added SW changes needed for dynamic compression (i.e. not M-Plane) which affects buffer sizing in DL & SSB
* Added per-spatial stream decompression (see xorif_set_cc_dl_iq_compression_per_ss())
* Added PRACH compression configuration API (see xorif_set_cc_iq_compression_prach())

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
