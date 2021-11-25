# XORIF-APP

## XORIF-APP Source Directory

* This directory contains the source files and Makefile for the ORAN-Radio-Interface Example Application (xorif-app)

* The xorif-app is an example application, which uses the libxorif library to interface with the Front Haul Interface hardware.

* The xorif-app can operate as either a "server" or a "client".
    * As a server, the xorif-app will provide a communication interface (via TCP/IP sockets) which will accept messages (e.g. from an xorif-app client).
    * As a client, the xorif-app can be use to connect with an xorif-app server. The client can operate in several modes, including "command line mode" or "file mode".
    * Only the xorif-app in "server mode" can communicate directly with the h/w (via the C libraries).

### Building

* The xorif-app executable is built as part of the PetaLinux build system, and can be found in the `/usr/bin/` directory of the target Linux installation.

* It can also be built separately using an appropriate SDK including the cross-compilation tools and dependencies (e.g. libmetal library).
    * Run: `make`

* A client-only version of the xorif-app executable can also be be built on a host Linux system, using native compilation tools and without dependency on the libmetal library.
    * Run: `make NO_HW=1`

### Usage

~~~
Usage: [-bhiv] [-c | -f <file> | -s] [-n <ip_addr>] [-p <port>] [-e <device>] {"<command> {<arguments>}"}
        -b Disable banner
        -c Client mode using the command line
        -e <device> Specified ethernet device (default eth0)
        -f <file> Client mode using the specified file
        -h Show help
        -i Auto-initialize (server mode only)
        -n <ip_addr> Specified IP address (for client mode) (defaults to localhost)
        -p <port> Specified port (defaults to 5001)
        -s Server mode (default)
        -v Verbose
        <command> {<arguments>} For command line mode only
~~~

* Server mode:
    * Typical usage: `xorif-app -s`
    * Use `-s` for server mode (note, this is the default and so not necessary in most cases)
    * Use `-p` to change the TCP/IP port number
    * Use `-e` to change the ethernet device name (when using time-stamping)
    * Use `-i` option to auto-initialize the server (instead of sending "init" command each time)

* Client mode:
    * Typical usage: `xorif-app -n 192.168.0.55 ...`
    * Use `-c` for "command-line mode" (runs commands specified on the command line, e.g. `xorif-app -c "get sw_version"`)
    * Use `-f` for "file mode" (runs commands specified in a text file, e.g. `xorif-app -f config.txt`)
    * Use `-n` to set the IP address (default is the localhost, i.e. 127.0.0.1)
    * Use `-p` to change the TCP/IP port number
    * The xorif-app server needs to be initialized before , so first time: `xorif-app -c "init"`

* Note on server/client ports
    * It is important that the xorif-app server and client are using the same port number to communicate
    * The xorif-app server will not start if the specified port is not available (e.g. in use by another xorif-app instance). In this case, an error is also displayed. Either close/kill the other xorif-app server, or select a different port number to continue.

### Command-Line Mode

* This mode allows basic configuration and interaction with the system using the xorif-app command line.

* Usage:
    * Ensure that an instance of the xorif-app "server" is running on the target hardware, e.g. `xorif-app -s`
    * Issue a command using the xorif-app "client" (optionally specifying the IP address and port number of the "server" if needed) and the required command e.g. `xorif-app -n 192.168.0.55 -c "get sw_version"`.

* It is also possible to send multiple commands on the same command line. Because of the way command line arguments are handled, each command should be enclosed in quotes, e.g. `xorif-app -n 192.168.0.55 -c "get sw_version" "get fhi_hw_version"`.

* See the section below on "Command Syntax" for details of the command syntax.

### Command Syntax

~~~
help : help [<topic>]
debug : debug <level = 0..2>
init : init [fhi] [<device_name>]
finish : finish [fhi]
get : get...
        get fhi_sw_version
        get fhi_hw_version
        get fhi_hw_internal_rev
        get [fhi_capabilities | fhi_caps]
        get fhi_cc_config <cc>
        get fhi_cc_alloc <cc>
        get fhi_stats <port>
        get fhi_alarms
        get fhi_state
        get fhi_enabled
set : set...
        set num_rbs <cc> <number_of_rbs>
        set numerology <cc> <numerology> <extended_cp = 0 | 1>
        set numerology_ssb <cc> <numerology> <extended_cp = 0 | 1>
        set time_advance <cc> <deskew> <advance_uplink> <advance_downlink> (deprecated)
        set ul_timing_params <cc> <delay_comp> <advance> <radio_ch_delay>
        set dl_timing_params <cc> <delay_comp_cp> <delay_comp_up> <advance>
        set ul_bid_forward <cc> <time>
        set ul_bid_forward_fine <cc> <symbols> <cycles>
        set ul_radio_ch_dly <cc> <delay> (deprecated)
        set [dl_iq_compression | dl_iq_comp] <cc> <width> <method> <mplane = 0 | 1>
        set [ul_iq_compression | ul_iq_comp] <cc> <width> <method> <mplane = 0 | 1>
        set [ssb_iq_compression | ssb_iq_comp] <cc> <width> <method> <mplane = 0 | 1>
        set [prach_iq_compression | prach_iq_comp] <cc> <width> <method> <mplane = 0 | 1>
        set dl_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>
        set ul_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>
        set ssb_sections_per_sym <cc> <number_of_sections> <number_of_ctrl_words>
        set frames_per_sym <cc> <number_of_frames>
        set frames_per_sym_ssb <cc> <number_of_frames>
        set dest_mac_addr <port> <address>
        set src_mac_addr <port> <address>
        set protocol <ECPRI | 1914.3> <VLAN = 0 | 1> <RAW | IPv4 | IPv6>
        set vlan <port> <id> <dei> <pcp>
        set eaxc_id <DU bits> <BS bits> <CC bits> <RU bits>
        set ru_ports <RU bits> <ss bits> <mask> <user_value> <prach_value> <ssb_value>
        set ru_ports_table_mode <mode>
        set ru_ports_table <address> <port> <type> [<number>]
        set system_constants <fh_decap_dly> [...]
reset : reset [fhi | bf] <mode>
clear : clear [fhi_alarms | fhi_stats | bf_alarms | bf_stats]
has : has [fhi | bf]
configure : configure [fhi] <cc>
enable : enable [fhi] <cc>
disable : disable [fhi] <cc>
read_reg : read_reg [fhi] <name>
read_reg_offset : read_reg_offset [fhi] <name> <offset>
write_reg : write_reg [fhi] <name> <value>
write_reg_offset : write_reg_offset [fhi] <name> <offset> <value>
dump_reg : dump_reg [fhi]
peek : peek <address>
poke : poke <address> <value>
quit : quit
~~~

* When executing a command, the app will echo the command and it's returned status, e.g. `set num_rbs 0 275 => status = 0`
    * Status value of 0 means good/pass and any non-zero value is an error code
    * Refer to the libxorif header file `xorif_api.h` for a list of error codes

* The xorif-app server needs to be initialized before it can correctly accept most commands
    * This can be done by sending the "init" command, e.g. `xorif-app -c "init"`
    * Note, the xorif-app server only needs to be initialized once (after starting, or after sending a the "finish" command). However, sending "init" multiple times is not a problem, and it will be ignored by the server.
    * The xorif-app will attempt to deduce the device name itself from the available platform devices. However, it can also be specified explicitly, e.g. `xorif-app -c "init fhi a0000000.oran_radio_if"`.
    * The server can also be set to "auto-initialize" with the `-i` option when starting the xorif-app server.

* Some commands also provide additional information, for example...

~~~
get fhi_caps => status = 0
max_cc = 8
num_eth_ports = 4
numerologies = 0x1F
extended_cp = false
iq_de_comp_methods = 0x13
iq_de_comp_bfp_widths = 0x5200
iq_de_comp_mod_widths = 0x3E
iq_comp_methods = 0x3
iq_comp_bfp_widths = 0x5200
no_framer_ss = 8
no_deframer_ss = 16
max_framer_ethernet_pkt = 8000
max_deframer_ethernet_pkt = 8000
max_subcarriers = 6600
max_data_symbols = 8
max_ctrl_symbols = 16
max_ul_ctrl_1kwords = 4
max_dl_ctrl_1kwords = 4
max_dl_data_1kwords = 8
max_ssb_ctrl_512words = 1
max_ssb_data_512words = 2
timer_clk_ps = 5000
num_unsolicited_ports = 1
du_id_limit = 4
bs_id_limit = 6
cc_id_limit = 3
ru_id_limit = 8
ss_id_limit = 5
ru_ports_map_width = 8
~~~

* Many of the commands map directly on to the libxorif library API calls. Others, such as "help" and "quit" are handled locally.

* The xorif-app commands also allow the user to read/write device registers, and even to peek/poke memory directly.

### File Mode

* This mode allows basic configuration and interaction with the system using a text file.

* Usage:
    * Ensure that an instance of the xorif-app "server" is running on the target hardware, e.g. `xorif-app -s`
    * Start an instance of the xorif-app "client", specifying the IP address of the "server" and the command e.g. `xorif-app -n 192.168.0.55 -f config1.txt`

* File format:
    * A plain text file
    * A maximum of one command per line
    * Line breaks in the middle of commands are not permitted
    * Blank lines are skipped
    * Hash characters ("#") can be used to provide comment lines in the file
    * Command syntax is the same as described above

* Example file (input):

~~~
# Initialize library
init
reset

# Check versions
get sw_version
get fhi_hw_version

# Configure CC[0] num_rbs=275, numerology=1
set num_rbs 0 275
set numerology 0 1 0
set time_advance 0 30 90 90
set dl_iq_compression 0 9 1 1
set ul_iq_compression 0 9 1 1
configure 0
enable 0

# Configure CC[1] num_rbs=100, numerology=0
set num_rbs 1 100
set numerology 1 0 0
set time_advance 1 30 90 90
set dl_iq_compression 1 9 1 1
set ul_iq_compression 1 9 1 1
configure 1
enable 1
~~~

* Example file (output):

~~~
init => status = 0
reset => status = 0
get sw_version => status = 0
fhi_version = 0x01010002
get fhi_hw_version => status = 0
result = 0x01010002
set num_rbs 0 275 => status = 0
set numerology 0 1 0 => status = 0
set time_advance 0 30 90 90 => status = 0
set dl_iq_compression 0 9 1 1 => status = 0
set ul_iq_compression 0 9 1 1 => status = 0
configure 0 => status = 0
enable 0 => status = 0
set num_rbs 1 100 => status = 0
set numerology 1 0 0 => status = 0
set time_advance 1 30 90 90 => status = 0
set dl_iq_compression 1 9 1 1 => status = 0
set ul_iq_compression 1 9 1 1 => status = 0
configure 1 => status = 0
enable 1 => status = 0
~~~

### File Mode Simple Starter and PG370 examples

* Simple uncompressed data example (input):
    * This is a good config to get started with:
    * Limit to one Component Carrier, 1 Spatial Stream and a limited number of RB'S.
    * Compared to the detail in the PG370, using the API removes the need to manually program these registers.

~~~
# Set initial state
init
reset

# Component carrier 0
set numerology 0 0 0
set num_rbs 0 20
set ul_iq_compression 0 0 0
set dl_iq_compression 0 0 0
set time_advance 0 160 100 100
configure 0
enable 0
~~~


* PG370 "Configuring the Core" Example file (input):

~~~
# Set initial state
init
reset

# Component carrier 0
set numerology 0 1 0
set num_rbs 0 275
set ul_iq_compression 0 9 1
set dl_iq_compression 0 9 1
set time_advance 0 30 90 90
configure 0
enable 0

# Component carrier 1
set numerology 1 1 0
set num_rbs 1 275
set ul_iq_compression 1 9 1
set dl_iq_compression 1 9 1
set time_advance 1 30 90 90
configure 1
enable 1
~~~

* PG370 "Reconfiguring the Core" Example file (input):

~~~
# Set initial state
init
reset

# Component carrier 0
set numerology 0 0 0
set num_rbs 0 275
set ul_iq_compression 0 9 1 1
set dl_iq_compression 0 9 1 1
set time_advance 0 30 90 90
configure 0
enable 0

# Component carrier 1
set numerology 1 1 0
set num_rbs 1 137
set ul_iq_compression 1 9 1 1
set dl_iq_compression 1 9 1 1
set time_advance 1 30 90 90
configure 1
enable 1

## Component carrier 2
set numerology 2 1 0
set num_rbs 2 137
set ul_iq_compression 2 9 1 1
set dl_iq_compression 2 9 1 1
set time_advance 2 30 90 90
configure 2
enable 2
~~~

### Debug Mode

* The "debug" command can be used to enable library's "trace" feature
    * Level 0: No trace, but errors are still reported
    * Level 1: The trace feature shows main API calls and register read/writes
    * Level 2: The trace feature shows debug extra information in addition to the main API calls and register read/writes
* This can be a very useful feature for debugging or checking the component carrier configurations. For example...

~~~
LIBXORIF> xorif_set_cc_numerology(0, 0, 0)
set numerology 0 0 0 => status = 0
LIBXORIF> xorif_set_cc_num_rbs(0, 20)
set num_rbs 0 20 => status = 0
LIBXORIF> xorif_set_cc_ul_iq_compression(0, 0, 0, 1)
set ul_iq_compression 0 0 0 => status = 0
LIBXORIF> xorif_set_cc_dl_iq_compression(0, 0, 0, 1)
set dl_iq_compression 0 0 0 => status = 0
LIBXORIF> xorif_set_cc_time_advance(0, 160, 100, 100)
set time_advance 0 160 100 100 => status = 0
LIBXORIF> xorif_configure_cc(0)
LIBXORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x0 (0)
LIBXORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x0 (0)
LIBXORIF> DEBUG: Configuration valid
LIBXORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_ENABLE (0xE004)[0:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_NUMRBS (0xE100)[0:8] <= 0x14 (20)
LIBXORIF> WRITE_REG: ORAN_CC_NUMEROLOGY (0xE100)[16:18] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SYMPERSLOT (0xE100)[24:24] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_NUMRBS (0xE900)[0:8] <= 0x14 (20)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_NUMEROLOGY (0xE900)[16:18] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SYMPERSLOT (0xE900)[24:24] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_SYM_NUM_INDEX (0xE114)[8:13] <= 0x4 (4)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_SYM_START_INDEX (0xE114)[0:5] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_CTRL_SYM_NUM_INDEX (0xE114)[16:21] <= 0x4 (4)
LIBXORIF> WRITE_REG: ORAN_CC_UL_CTRL_SYM_NUM_INDEX (0xE114)[24:29] <= 0x4 (4)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_NUM_DATA_SYM_PER_CC (0xE914)[8:13] <= 0x4 (4)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_DATA_SYM_START_INDEX (0xE914)[0:5] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_NUM_SYM_PER_CC (0xE914)[16:21] <= 0x4 (4)
LIBXORIF> WRITE_REG: ORAN_CC_DL_UD_IQ_WIDTH (0xE11C)[0:3] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_UD_COMP_METH (0xE11C)[4:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_MPLANE_UDCOMP_PARAM (0xE11C)[8:8] <= 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_UL_UD_IQ_WIDTH (0xE118)[0:3] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_UL_UD_COMP_METH (0xE118)[4:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_UL_MPLANE_UDCOMP_PARAM (0xE118)[8:8] <= 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_UD_IQ_WIDTH (0xE91C)[0:3] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_UD_COMP_METH (0xE91C)[4:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_MPLANE_UDCOMP_PARAM (0xE91C)[8:8] <= 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_PRACH_UD_IQ_WIDTH (0xE920)[0:3] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_PRACH_UD_COMP_METH (0xE920)[4:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_PRACH_MPLANE_UDCOMP_PARAM (0xE920)[8:8] <= 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_NUM_CTRL_PER_SYMBOL_DL (0xE160)[0:15] <= 0x40 (64)
LIBXORIF> WRITE_REG: ORAN_CC_DL_CTRL_OFFSETS (0xE104)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_NUM_CTRL_PER_SYMBOL_UL (0xE164)[0:15] <= 0x40 (64)
LIBXORIF> WRITE_REG: ORAN_CC_UL_CTRL_OFFSETS (0xE10C)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_UL_BASE_OFFSET (0xE140)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC (0xE960)[0:15] <= 0x5 (5)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_CTRL_OFFSETS (0xE904)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE500)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE504)[0:15] <= 0xAD (173)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE508)[0:15] <= 0x15A (346)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE50C)[0:15] <= 0x207 (519)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED00)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED04)[0:15] <= 0x7F (127)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED08)[0:15] <= 0xFE (254)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED0C)[0:15] <= 0x17D (381)
LIBXORIF> WRITE_REG: ORAN_CC_MODVALS_DL (0xE168)[0:15] <= 0x100 (256)
LIBXORIF> WRITE_REG: ORAN_CC_MODVALS_UL (0xE16C)[0:15] <= 0x100 (256)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SECTS_X_SYMBOLS (0xE968)[0:15] <= 0x14 (20)
LIBXORIF> WRITE_REG: ORAN_CC_DL_SETUP_D_CYCLES (0xE138)[0:16] <= 0x33E5 (13285)
LIBXORIF> WRITE_REG: ORAN_CC_DL_SETUP_C_CYCLES (0xE134)[0:16] <= 0x1D92 (7570)
LIBXORIF> WRITE_REG: ORAN_CC_DL_SETUP_C_ABS_SYMBOL (0xE130)[0:11] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_UL_SETUP_D_CYCLES (0xE128)[0:16] <= 0x37CD (14285)
LIBXORIF> WRITE_REG: ORAN_CC_UL_SETUP_C_CYCLES (0xE124)[0:16] <= 0xA0A (2570)
LIBXORIF> WRITE_REG: ORAN_CC_UL_SETUP_C_ABS_SYMBOL (0xE120)[0:11] <= 0x3 (3)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SETUP_D_CYCLES (0xE938)[0:16] <= 0x33E5 (13285)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SETUP_C_CYCLES (0xE934)[0:16] <= 0x1D92 (7570)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SETUP_C_ABS_SYMBOL (0xE930)[0:11] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_UL_BIDF_C_CYCLES (0xE148)[0:16] <= 0x11DA (4570)
LIBXORIF> WRITE_REG: ORAN_CC_UL_BIDF_C_ABS_SYMBOL (0xE144)[0:11] <= 0x3 (3)
LIBXORIF> WRITE_REG: ORAN_CC_RELOAD (0xE000)[0:7] <= 0x1 (1)
configure 0 => status = 0
LIBXORIF> xorif_enable_cc(0)
LIBXORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_ENABLE (0xE004)[0:7] <= 0x1 (1)
enable 0 => status = 0
~~~

---

Copyright (C) 2019 - 2021  Xilinx, Inc.  All rights reserved.
