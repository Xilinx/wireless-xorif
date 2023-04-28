# Xilinx O-RAN Radio Interface Software Application (XORIF-APP)

* The xorif-app is an test/demo application, which uses the hardware device libraries (e.g. libxorif.so) to interface with the O-RAN Radio Interface IP core(s).


* Prior to the 2022.2 release, the xorif-app application was a compiled executable. However, since the 2022.2 release a Python-based version of the application is also provided. There are some minor differences in how the legacy application and the Python application operate, but the fuctionality is more or less equivalent.

* The xorif-app can operate as either a "server" or a "client".
    * As a server, the xorif-app will provide a communication interface (via TCP/IP sockets) which will accept messages (e.g. from an xorif-app client).
    * As a client, the xorif-app can be use to connect with an xorif-app server. The client can operate in several modes, including "command line mode", "file mode" or "interactive".
    * Only the xorif-app server can communicate directly with the hardware device (via the device libraries).

## Building (Legacy Application)

* The legacy xorif-app executable is built as part of the PetaLinux build, and can be found in the `/usr/bin/` directory of the target Linux installation.

* It can also be built separately using an appropriate SDK including the cross-compilation tools and dependencies (e.g. libmetal library).
    * Run: `make`

* A client-only version of the xorif-app executable can also be be built on a host Linux system, using native compilation tools and without dependency on the libmetal library.
    * Run: `make NO_HW=1`

## Building (Python Application)

* The Python xorif-app application is provided as part of the PetaLinux build, and can be found in the `/usr/bin/` directory of the target Linux installation.

* Unlike the legacy xorif-app application, there are separate scripts for the server and client.
    * The "server" is `xorif-app-server.py`
    * The "client" is `xorif-app-client.py`

* It is possible to run the client application on another host Linux system, although you will need to `pip install` a couple of Python packages (Pyro4 and CFFI)

* The Python application requires a couple of Python packages to be installed, and requires Python 3.6.5 or greater.
    * Run: `pip install Pyro4`
    * Run: `pip install cffi`

## Usage (Legacy Application)

* Usage:

~~~
Usage: xorif-app [-c | -f <file> |-I | -s] [-n <ip_addr>] [-p <port>] [<options>] {"<command> {<arguments>}"}
        -h Show help
        -c Client mode using the command line
        -f <file> Client mode using the specified file
        -I Client mode using interactive input (type exit to leave)
        -s Server mode (default)
        -n <ip_addr> Specified IP address (client mode only) (defaults to localhost)
        -p <port> Specified port (defaults to 5001)
        -b Disable banner on start
        -i Auto-initialize (server mode only)
        -v Verbose
        -V Display version of the application
        <command> {<arguments>} For command line mode only
~~~

* Server mode:
    * Typical usage: `xorif-app -s`
    * Use `-s` for server mode (note, this is the default and so not necessary in most cases)
    * Use `-p` to change the TCP/IP port number

* Client mode:
    * Typical usage: `xorif-app -n 192.168.0.55`
    * Use `-c` for "command-line mode" (runs commands specified on the command line, e.g. `xorif-app -c "get sw_version"`)
    * Use `-f` for "file mode" (runs commands specified in a text file, e.g. `xorif-app -f config.txt`)
    * Use `-I` for "interactive mode" (runs commands entered at a prompt, use "exit" to leave)
    * Use `-n` to set the IP address (default is the localhost, i.e. 127.0.0.1)
    * Use `-p` to change the TCP/IP port number

* Note on server/client ports
    * It is important that the xorif-app server and client are using the same port number to communicate
    * The xorif-app server will not start if the specified port is not available (e.g. in use by another application). In this case, an error is also displayed. Either close/kill the other xorif-app server, or select a different port number to continue.

## Usage (Python Application)

* Usage:

~~~
usage: xorif-app-server [-h] [-V] [-v VERBOSE] [-N NET] [-P PORT] [--fhi]
                        [--no-fhi] [--ocp] [--no-ocp] [--oprach] [--no-oprach]

Python O-RAN Radio Interface server

optional arguments:
  -h, --help            show this help message and exit
  -V, --Version         show program's version number and exit
  -v VERBOSE, --verbose VERBOSE
                        set verbosity level (0 = quiet, 1 = normal, 2 = high)
  -N NET, --net NET     Network device to use for IP address (default is
                        localhost IP address))
  -P PORT, --port PORT  port number (default is 9090)
  --fhi                 with FHI support (default is yes)
  --no-fhi              without FHI support
  --ocp                 with OCP support (default is yes)
  --no-ocp              without OCP support
  --oprach              with OPRACH support (default is no)
  --no-oprach           without OPRACH support
~~~

~~~
usage: xorif-app-client [-h] [-V] [-v VERBOSE] [-H HOST] [-P PORT] [-I]
                        [-f FILE [FILE ...]] [-c CMD [CMD ...]]

Python O-RAN Radio Interface client

optional arguments:
  -h, --help            show this help message and exit
  -V, --Version         show program's version number and exit
  -v VERBOSE, --verbose VERBOSE
                        set verbosity level (0 = quiet, 1 = normal, 2 = high)
  -H HOST, --host HOST  IP address (default is localhost)
  -P PORT, --port PORT  port number (default is 9090)
  -I, --interactive     process commands in interactive mode
  -f FILE [FILE ...], --file FILE [FILE ...]
                        process commands from file
  -c CMD [CMD ...], --cmd CMD [CMD ...]
                        process command directly
~~~

* Server:
    * Typical usage: `xorif-app-server.py`
    * Typical usage: `xorif-app-server.py -N eth0 -P 9090`
    * Use `-N` or `--net` to select the network device used for determining the IP address (default is to use localhost IP address, i.e. 127.0.0.1)
    * Use `-P` or `--port` to set the TCP/IP port number

* Client:
    * Typical usage: `xorif-app-client.py -c "init"`
    * Typical usage: `xorif-app-client.py -c "debug 2"`
    * Typical usage: `xorif-app-client.py -f test_100mhz_1cc.config`
    * Typical usage: `xorif-app-client.py -H 192.168.10.10 -P 9090  -f test_100mhz_1cc.config`
    * Use `-H` or `--host` to set the IP address (default is the localhost, i.e. 127.0.0.1)
    * Use `-P` or `--port` to set the TCP/IP port number
    * The verbosity can be set by the `-v` option. The default is to display errors and warnings only

* Note on server/client ports
    * It is important that the xorif-app server and client are using the same port number to communicate
    * The xorif-app server will not start if the specified port is not available (e.g. in use by another xorif-app instance). In this case, an error is also displayed. Either close/kill the other xorif-app server, or select a different port number to continue.
    * The default IP address used is the localhost. However, when using localhost, the server only accepts client connections from the localhost. If you want to connect from a remote system, then you should specify the actual IP address. You can do this by specifying a network device on the server with the `--net` / `-N` option, and the IP address on the client with the `--host` / `-H` option.

## Command-Line Mode

* This mode allows basic configuration and interaction with the system using the xorif-app command line.

* Usage:
    * Ensure that an instance of the xorif-app "server" is running on the target hardware, e.g. `xorif-app -s` or `xorif-app-server.py`
    * Start the xorif-app "client" (optionally specifying the IP address and port number of the "server" if needed) and the required command to execute, e.g. `xorif-app -c "get sw_version"` or `xorif-app-client.py -c "get sw_version"`
    * Note, because of the way command line arguments are handled, multi-word commands should be enclosed in quotes.

* It is also possible to send multiple commands on the same command line, e.g. `xorif-app -c "get sw_version" "get fhi_hw_version"` or `xorif-app-client.py -c "get sw_version" "get fhi_hw_version"`

* See the section below on "Command Syntax" for details of the command syntax.

## Interactive Mode

* This mode allows commands to be entered at a prompt, similar to the "command line mode".

* Usage:
    * Ensure that an instance of the xorif-app "server" is running on the target hardware, e.g. `xorif-app -s` or `xorif-app-server.py`
    * Start the xorif-app "client" in interactive mode (optionally specifying the IP address and port number of the "server" if needed), i.e. `xorif-app -I` or `xorif-app-client.py -I`
    * Enter desired commands at the prompt
    * Enter "exit" (or Ctrl-C) to leave interactive mode

* See the section below on "Command Syntax" for details of the command syntax.

## Command Syntax

* The xorif-app provides basic help on the available commands and command syntax.

* Run `xorif-app -c help` or `xorif-app-client.py -c help` for available commands.

~~~
Available commands...
    activate            : Activate HW, transitioning to operational state
    clear               : Clear various status, alarms, etc.
    configure           : Program component carrier configuration
    deactivate          : Deactivate HW, transitioning to initialized state
    debug               : Set debugging level
    devices             : List devices accessible as Pyro proxies
    disable             : Disable component carrier
    dump                : Dump debug information
    enable              : Enable component carrier
    exit                : Exit the client script / interactive session
    finish              : Close-down device driver libraries
    get                 : Get various configuration and status data from a device
    has                 : Check for the presence of a device
    help                : Provide command help and syntax
    init                : Start-up device driver libraries
    monitor             : Configure / use monitor block
    read_reg            : Read device registers
    read_reg_offset     : Read device registers (with offsets)
    reset               : Reset devices
    run                 : Run a command file
    set                 : Set various configuration data for device
    slv                 : SLV test commands
    terminate           : Terminate server
    trigger             : Trigger HW update, e.g. causing update from 'current' to 'next'
    version             : Display version of the application
    wait                : Wait for a short time
    write_reg           : Write device registers
    write_reg_offset    : Write device registers (with offsets)
~~~

* Run `xorif-app -c "help <topic>"` o `xorif-app-client.py -c "help <topic>"` for help on a particular topic, e.g.

~~~
get : Get various configuration and status data from a device
  usage: get (fhi_sw_version | fhi_hw_version | fhi_hw_internal_rev)
  usage: get (fhi_capabilities | fhi_caps)
  usage: get fhi_cc_config <cc>
  usage: get fhi_cc_alloc <cc>
  usage: get fhi_stats <port>
  usage: get (fhi_alarms | fhi_state | fhi_enabled)
  ...
~~~

* When executing a command, the app will echo the command and it's returned status, e.g. `set num_rbs 0 275 => status = 0`
    * Status value of 0 means good/pass and any non-zero value is an error code
    * Refer to the library's API header file (e.g. `xorif_api.h`) for a list of error codes

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

* Many of the commands map directly on to the library API calls. Others, such as "help" and "exit" are handled locally.

* The xorif-app commands also allow the user to read/write device registers directly.

## File Mode

* This mode allows basic configuration and interaction with the system using a text file.

* Usage:
    * Ensure that an instance of the xorif-app "server" is running on the target hardware, e.g. `xorif-app -s` or `xorif-app-server.py`
    * Start the xorif-app "client" (optionally specifying the IP address and port number of the "server" if needed) and the required file to process, e.g. `xorif-app -f config1.txt` or `xorif-app-client.py -f config.txt`

* File format:
    * A plain text file
    * A maximum of one command per line
    * Line breaks in the middle of commands are not permitted
    * Blank lines are skipped
    * Hash characters ("#") can be used to provide comment lines in the file (everything after the "#" is skipped)
    * Command syntax is the same as described previously

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

## File Mode Simple Starter and PG370 examples

* Simple uncompressed data example (input):
    * This is a good configuration to get started with:
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

## Debug Mode

* The "debug" command can be used to enable library's "trace" feature
    * Level 0: No trace, but errors are still reported
    * Level 1: The trace feature shows main API calls and register read/writes
    * Level 2: The trace feature shows debug extra information in addition to the main API calls and register read/writes
* This can be a very useful feature for debugging or checking the component carrier configurations. For example...

~~~
XORIF> xorif_set_cc_numerology(0, 0, 0)
set numerology 0 0 0 => status = 0
XORIF> xorif_set_cc_num_rbs(0, 20)
set num_rbs 0 20 => status = 0
XORIF> xorif_set_cc_ul_iq_compression(0, 0, 0, 1)
set ul_iq_compression 0 0 0 => status = 0
XORIF> xorif_set_cc_dl_iq_compression(0, 0, 0, 1)
set dl_iq_compression 0 0 0 => status = 0
XORIF> xorif_set_cc_time_advance(0, 160, 100, 100)
set time_advance 0 160 100 100 => status = 0
XORIF> xorif_configure_cc(0)
XORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x0 (0)
XORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x0 (0)
XORIF> DEBUG: Configuration valid
XORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_ENABLE (0xE004)[0:7] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_NUMRBS (0xE100)[0:8] <= 0x14 (20)
XORIF> WRITE_REG: ORAN_CC_NUMEROLOGY (0xE100)[16:18] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SYMPERSLOT (0xE100)[24:24] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_NUMRBS (0xE900)[0:8] <= 0x14 (20)
XORIF> WRITE_REG: ORAN_CC_SSB_NUMEROLOGY (0xE900)[16:18] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_SYMPERSLOT (0xE900)[24:24] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_SYM_NUM_INDEX (0xE114)[8:13] <= 0x4 (4)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_SYM_START_INDEX (0xE114)[0:5] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_CTRL_SYM_NUM_INDEX (0xE114)[16:21] <= 0x4 (4)
XORIF> WRITE_REG: ORAN_CC_UL_CTRL_SYM_NUM_INDEX (0xE114)[24:29] <= 0x4 (4)
XORIF> WRITE_REG: ORAN_CC_SSB_NUM_DATA_SYM_PER_CC (0xE914)[8:13] <= 0x4 (4)
XORIF> WRITE_REG: ORAN_CC_SSB_DATA_SYM_START_INDEX (0xE914)[0:5] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_NUM_SYM_PER_CC (0xE914)[16:21] <= 0x4 (4)
XORIF> WRITE_REG: ORAN_CC_DL_UD_IQ_WIDTH (0xE11C)[0:3] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_UD_COMP_METH (0xE11C)[4:7] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_MPLANE_UDCOMP_PARAM (0xE11C)[8:8] <= 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_UL_UD_IQ_WIDTH (0xE118)[0:3] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_UL_UD_COMP_METH (0xE118)[4:7] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_UL_MPLANE_UDCOMP_PARAM (0xE118)[8:8] <= 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_SSB_UD_IQ_WIDTH (0xE91C)[0:3] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_UD_COMP_METH (0xE91C)[4:7] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_MPLANE_UDCOMP_PARAM (0xE91C)[8:8] <= 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_PRACH_UD_IQ_WIDTH (0xE920)[0:3] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_PRACH_UD_COMP_METH (0xE920)[4:7] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_PRACH_MPLANE_UDCOMP_PARAM (0xE920)[8:8] <= 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_NUM_CTRL_PER_SYMBOL_DL (0xE160)[0:15] <= 0x40 (64)
XORIF> WRITE_REG: ORAN_CC_DL_CTRL_OFFSETS (0xE104)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_NUM_CTRL_PER_SYMBOL_UL (0xE164)[0:15] <= 0x40 (64)
XORIF> WRITE_REG: ORAN_CC_UL_CTRL_OFFSETS (0xE10C)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_UL_BASE_OFFSET (0xE140)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC (0xE960)[0:15] <= 0x5 (5)
XORIF> WRITE_REG: ORAN_CC_SSB_CTRL_OFFSETS (0xE904)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE500)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE504)[0:15] <= 0xAD (173)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE508)[0:15] <= 0x15A (346)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE50C)[0:15] <= 0x207 (519)
XORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED00)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED04)[0:15] <= 0x7F (127)
XORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED08)[0:15] <= 0xFE (254)
XORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED0C)[0:15] <= 0x17D (381)
XORIF> WRITE_REG: ORAN_CC_MODVALS_DL (0xE168)[0:15] <= 0x100 (256)
XORIF> WRITE_REG: ORAN_CC_MODVALS_UL (0xE16C)[0:15] <= 0x100 (256)
XORIF> WRITE_REG: ORAN_CC_SSB_SECTS_X_SYMBOLS (0xE968)[0:15] <= 0x14 (20)
XORIF> WRITE_REG: ORAN_CC_DL_SETUP_D_CYCLES (0xE138)[0:16] <= 0x33E5 (13285)
XORIF> WRITE_REG: ORAN_CC_DL_SETUP_C_CYCLES (0xE134)[0:16] <= 0x1D92 (7570)
XORIF> WRITE_REG: ORAN_CC_DL_SETUP_C_ABS_SYMBOL (0xE130)[0:11] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_UL_SETUP_D_CYCLES (0xE128)[0:16] <= 0x37CD (14285)
XORIF> WRITE_REG: ORAN_CC_UL_SETUP_C_CYCLES (0xE124)[0:16] <= 0xA0A (2570)
XORIF> WRITE_REG: ORAN_CC_UL_SETUP_C_ABS_SYMBOL (0xE120)[0:11] <= 0x3 (3)
XORIF> WRITE_REG: ORAN_CC_SSB_SETUP_D_CYCLES (0xE938)[0:16] <= 0x33E5 (13285)
XORIF> WRITE_REG: ORAN_CC_SSB_SETUP_C_CYCLES (0xE934)[0:16] <= 0x1D92 (7570)
XORIF> WRITE_REG: ORAN_CC_SSB_SETUP_C_ABS_SYMBOL (0xE930)[0:11] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_UL_BIDF_C_CYCLES (0xE148)[0:16] <= 0x11DA (4570)
XORIF> WRITE_REG: ORAN_CC_UL_BIDF_C_ABS_SYMBOL (0xE144)[0:11] <= 0x3 (3)
XORIF> WRITE_REG: ORAN_CC_RELOAD (0xE000)[0:7] <= 0x1 (1)
configure 0 => status = 0
XORIF> xorif_enable_cc(0)
XORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_ENABLE (0xE004)[0:7] <= 0x1 (1)
enable 0 => status = 0
~~~

---

Copyright (C) 2019 - 2023 Advanced Micro Devices, Inc. All rights reserved.
