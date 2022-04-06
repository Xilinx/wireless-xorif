# Change Log

## Release 2022.1
* Modified Yocto recipes for 'honister' release
* Makefile improvements
* Split doxygen-generated documentation into separate configs
* Added "set protocol_alt" command which just sets protocol, and not the packet filter configuration
* Added "set packet_filter" command which is used to configure the packet filter
* Added "set bf_fft_shift_mode" command which is used to set teh FFT shift mode
* Added "wait" command (used to script a short delay, e.g. in a configuration file)
* Added "interactive mode" (allows commands to be entered at prompt)
* Added "exit" command (used to exit the interactive mode)
* Removed the unused / legacy eCPRI 'one-way delay measurement' code
* Overhauled application help command
* Added "example" commands for demo purposes
* Added "run" command
* Enhanced "schedule_bf" commands (supports default, custom, with/without padding)
* Add API for "ul_data_arrival_offset" (debug)
* Added per-spatial stream decompression (see "set per_ss_decompression" command)
* Add "get bf_error_flags" command
* Modified "dump" debug commands

## Release 2021.2
* Improved debug logging
* Added RU port mapping table: 'set ru_ports_table_mode', 'set ru_ports_table'
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