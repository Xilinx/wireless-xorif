# XORIF-APP

## XORIF-APP Source Directory

* This directory contains the source files and Makefile for the ORAN-Radio-Interface Example Application (xorif-app)

* The xorif-app is an example application, which uses the libxorif library to interface with the Front Haul Interface and Beam-Former hardware. The xorif-app also demonstrates the handling of some eCPRI management messages, such as OWDM (One-Way Delay Measurement).

* The xorif-app can operate as either a "server" or a "client".
	* As a server, the xorif-app will provide a communication interface (via TCP/IP sockets) which will accept messages (e.g. from an xorif-app client).
	* As a client, the xorif-app can be use to connect with an xorif-app server. The client can operate in several modes, including "command line mode", "menu mode", or "file mode".
	* Only the xorif-app server mode can communicate directly with the h/w via the libxorif library.

### Building

* The xorif-app executable is built as part of the PetaLinux build system, and can be found in the `/usr/bin/` directory of the target Linux installation.

* It can also be built separately using an appropriate SDK including the cross-compilation tools and dependencies (e.g. libmetal library).
    * Run: `make`

* A client-only version of the xorif-app executable can also be be built on a host Linux system, using native compilation tools and without dependency on the libmetal library.
    * Run: `make NO_HW=1`

### Usage

~~~
	Usage: [-hv] [-c | -f <file> | -m | -s] [-n <ip_addr>] [-p <port>] [-e <device>] {"<command> {<arguments>}"}
        -c Client mode using the command line
        -e <device> Specified ethernet device (default eth0)
        -f <file> Client mode using the specified file
        -h Show help
        -m Client mode using console menu
        -n <ip_addr> Specified IP address (for client mode) (defaults to localhost)
        -p <port> Specified port (defaults to 5001)
        -s Server mode (default)
        -v Verbose
        <commands> {<arguments>} For command line mode only
~~~

* Server mode:
    * Typical usage: `xorif-app -s`
	* Use `-s` for server mode (note, this is the default and so not necessary in most cases)
	* Use `-p` to change the TCP/IP port number
	* Use `-e` to change the ethernet device name (when using time-stamping)
	
* Client mode:
	* Typical usage: `xorif-app -n 192.168.0.55 -m`
	* Use `-m` for "menu mode" (this is a simple console-based menu system)
	* Use `-c` for "command-line mode" (runs commands specified on the command line, e.g. `xorif-app -c "get bf_hw_version" "get fhi_hw_version"`)
	* Use `-f` for "file mode" (runs commands specified in a text file)
	* Use `-n` to set the IP address (default is the localhost, i.e. 127.0.0.1)
	* Use `-p` to change the TCP/IP port number

### Menu Mode

* This mode allows basic configuration and interaction with the system using a simple text-based menu.

* There are several different sub-menus, which can be easily navigated using the console.
	* "XORIF-APP Main Menu"
	* "XORIF-APP Component Carrier Configuration Menu"
	* "XORIF-APP Ethernet Configuration Menu"
	* "XORIF-APP Read/Write Register Menu"

* Usage:
	* Ensure that an instance of the xorif-app "server" is running on the target hardware, e.g. `xorif-app -s`.
	* Start an instance of the xorif-app "client", specifying the IP address of the "server", e.g. `xorif-app -n 192.168.0.55 -m`.
	* Use the menus to configure and interact with the system.

* Example:
~~~
	XORIF-APP Main Menu
	===================
	  1) Versions
	  2) Capabilities
	  3) Component carrier configuration
	  4) Ethernet configuration
	  5) Read/write registers and memory
	  6) Read stats / counters
	  7) Debug trace
	  8) Reset FHI
	100) Exit
	
	Enter :
~~~

### Command-Line Mode

* This mode allows basic configuration and interaction with the system using the xorif-app command line.

* Usage:
	* Ensure that an instance of the xorif-app "server" is running on the target hardware, e.g. `xorif-app -s`
	* Start an instance of the xorif-app "client", specifying the IP address of the "server" and the command e.g. `xorif-app -n 192.168.0.55 -c "get sw_version"`.

* It is also possible to send multiple commands on the same command line. Because of the way command line arguments are handled, each command should be enclosed in quotes, e.g. `xorif-app -n 192.168.0.55 -c "get sw_version" "get fhi_hw_version"`.

* See the section below on "Command Syntax" for details of the command syntax.

### Command Syntax

~~~
	help : help [<topic>]
	init : init [<device> <device>]
	finish : finish
	reset : reset [fhi]
	has : has [fhi]
	get : get...
			get sw_version
			get fhi_hw_version
			get fhi_hw_revision
			get capabilities
			get fhi_caps
			get cc_config <cc>
			get fhi_cc_alloc <cc>
			get fhi_stats
			get state
			get fhi_alarms
	set : set...
			set num_rbs <cc> <number of rbs>
			set numerology <cc> <numerology> <extended CP>
			set time_advance <cc> <deskew> <advance up-link> <advance down-link>
			dl_iq_compression <cc> <width> <compression method>
			ul_iq_compression <cc> <width> <compression method>
			bw_compression <cc> <width> <compression method>
			dest_mac_addr <port> <address>
			src_mac_addr <port> <address>
	clear : clear...
			clear fhi_alarms
	configure : configure <cc>
	enable : enable <cc>
	disable : disable <c>
	read_reg : read_reg [fhi] <name>
	read_reg_offset : read_reg_offset [fhi] <name> <offset>
	write_reg : write_reg [fhi] <name> <value>
	write_reg_offset : write_reg_offset [fhi] <name> <offset> <value>
	quit : quit
	ecpri : (Use 'ecpri help' for additional help and usage information)
	peek : peek <address>
	poke : poke <address> <value>
	debug : debug <level>
~~~

* When executing a command, the app will echo the command and it's status, e.g. `set num_rbs 0 275 => status = 0`
	* Status value of 0 means good/pass and any non-zero value is an error code
	
* Some commands also provide additional information, for example...
~~~
	get fhi_caps => status = 0
	Num framer antennas = 4
	Num deframer antennas = 4
	Num ethernet ports = 4
	Max component carriers = 4
	Max number down-link data symbols = 6
	Max size framer ethernet payload = 1024
	Max size deframer ethernet payload = 1024
	Maximum sub-carriers = 6600
	Maximum num ctrl symbols per CC = 13
	Maximum up-link ctrl 1k words = 4
	Maximum down-link ctrl 1k words = 4
	Maximum down-link data 1k words = 6
	Timer clk period in pico-seconds = 2500
	Number of unsolicited framer ports = 1
	Number of PRACH framer ports = 1
~~~

* Many of the commands map directly on to the libxorif library API calls. Others, such as "help" and "quit" are handled locally.

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
	# Check versions
	get sw_version
	get fhi_hw_version
	get bf_hw_revision
	
	# Configure CC#0 num_rbs=275, numerology=1
	set num_rbs 0 275
	set numerology 0 1 0
	set time_advance 0 30 90 90
	set dl_iq_compression 0 9 1
	set ul_iq_compression 0 9 1
	configure 0
	
	# Configure CC#1 num_rbs=100, numerology=0
	set num_rbs 1 100
	set numerology 1 0 0
	set time_advance 1 30 90 90
	set dl_iq_compression 1 9 1
	set ul_iq_compression 1 9 1
	configure 1
~~~

* Example file (output):
~~~
	init => status = 0
	get sw_version => status = 0
	result = 0x01000000
	get fhi_hw_version => status = 0
	result = 0x01000000
	get bf_hw_revision => status = 0
	result = 0x00000000
	set num_rbs 0 275 => status = 0
	set numerology 0 1 0 => status = 0
	set time_advance 0 30 90 90 => status = 0
	set dl_iq_compression 0 9 1 => status = 0
	set ul_iq_compression 0 9 1 => status = 0
	configure 0 => status = 0
	set num_rbs 1 100 => status = 0
	set numerology 1 0 0 => status = 0
	set time_advance 1 30 90 90 => status = 0
	set dl_iq_compression 1 9 1 => status = 0
	set ul_iq_compression 1 9 1 => status = 0
	configure 1 => status = 0
~~~

### File Mode Simple Starter and PG370 examples

* Simple Uncompressed data example (input):
	* This is a good config to get started with. Limit to one Component Carrier, 1 Spatial Stream and a limited number of RB'S.
	* Compared to the detail in the PG370, using the API removes the need to manually program these registers.
	* Set debug to 1 to see the complete listing of all writes to the core for a deeper understanding of configuration.
~~~
set debug                1
set cc_numerology        0 0 0
set cc_num_rbs           0 20
set cc_ul_iq_compression 0 0 0
set cc_dl_iq_compression 0 0 0
set cc_time_advance      0 160 100 100
configure_cc             0
~~~

* PG370 "Configuring the Core" Example file (input):
~~~
set cc_numerology        0 1 0
set cc_num_rbs           0 275
set cc_ul_iq_compression 0 9 1
set cc_dl_iq_compression 0 9 1
set cc_time_advance      0 30 90 90
configure_cc             0

set cc_numerology        1 1 0
set cc_num_rbs           1 275
set cc_ul_iq_compression 1 9 1
set cc_dl_iq_compression 1 9 1
set cc_time_advance      1 30 90 90
configure_cc             1
~~~
* PG370 "Reconfiguring the Core" Example file (input):
~~~
set cc_numerology        0 0 0
set cc_num_rbs           0 275
set cc_ul_iq_compression 0 9 1
set cc_dl_iq_compression 0 9 1
set cc_time_advance      0 30 90 90
configure_cc             0

set cc_numerology        1 1 0
set cc_num_rbs           1 137
set cc_ul_iq_compression 1 9 1
set cc_dl_iq_compression 1 9 1
set cc_time_advance      1 30 90 90
configure_cc             1

set cc_numerology        2 1 0
set cc_num_rbs           2 137
set cc_ul_iq_compression 2 9 1
set cc_dl_iq_compression 2 9 1
set cc_time_advance      2 30 90 90
configure_cc             2
~~~


---

Copyright (C) 2019 - 2020  Xilinx, Inc.  All rights reserved.
