# Change Log

## Release 2021.2
* Improved debug logging
* Added RU port mapping table: 'set ru_ports_table_mmode', 'set ru_ports_table'
* Added command to set system constants: 'set system_constants'
* 'set time_advance' now take doubles rather than integer
* Added command to set 'ul_radio_ch_dly' per component carrier
* Removed legacy "ecpri" commands
* Improved code coverage testing
* Changes for new and deprecated APIs

## Release 2021.1
* Refactored library code
* Changes for code coverage tools
* Additional capabilities added to xorif_caps
* PRACH compression configuration added to xorif_cc_config
* Uplink beam-id forward time advance added to xorif_cc_config
* Updated for modified xorif_init() API
* Updated for new xorif_set_ul_bid_forward() API
* Updated for modified xorif_set_cc_dl_iq_compression() API
* Updated for modified xorif_set_cc_ul_iq_compression() API
* Updated for modified xorif_set_cc_iq_compression_ssb() API
* Updated for new xorif_set_cc_iq_compression_prach() API
* Updated for modified xorif_get_fhi_hw_internal_rev() API
* Updated for modified xorif_set_fhi_vlan_tag() API
* Updated for modified xorif_set_ru_ports_alt1() API added (LTE option)