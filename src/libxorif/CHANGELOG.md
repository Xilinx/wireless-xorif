# Change Log

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