# Xilinx O-RAN Radio Interface Software Driver Library (LIBXORIF)

* The C library (libxorif) is a Linux shared library that provides the functions needed to access, configure and manage the O-RAN Radio Interface IP core

* Release 2023.1 has the option of the ORAN Channel Processor (OCP) integrated with ORAN Radio Interface (ORIF), which uses common s/w driver library (libxorif), but with separate APIs.

## API

* The C library API is documented [here](../../doc/libxorif_html/group__API.html)

    * The main header file for the ORAN Radio Interface (ORIF) is `xorif_api.h`

    * The main header for the integrated ORAN Channel Processor (OCP) is `xocp_api.h`

* The example application (xorif-app) demonstrates the use of the library API, and is a good starting point for reference

## Building

* The libxorif library is built as part of the PetaLinux build system, and can be found in the `/usr/lib/` directory of the target Linux installation

* It can also be built separately using an appropriate SDK including the cross-compilation tools and dependencies (e.g. libmetal library)

    * Run: `make`

## Python Bindings

* The Python module `pylibxorif.py` provides (Python 3) bindings for the ORAN Radio Interface C library
    * The `pylibxorif.py` module can be found in the current directory, and it is also installed as part of the PetaLinux build, and can be found in the `/usr/share/xorif` directory
    * The Python bindings use CFFI and can be used directly, or in conjunction with other libraries such as Pyro4 which allows remote procedure calls to Python objects
    * The Pyro4 and CFFI libraries are included as part of the Petalinux build, or can be installed manually with `pip install`
    * The Python bindings are provided for "ease of use". The Python API is largely the same as the C API, but more "Pythonic", e.g. allowing "dicts" to be passed in rather than pointers to structures.
* Similarly, there are Python bindings for the integrated ORAN Channel Processor which can be found in `pylibxocp.py`

## Usage

* The basic use of the library API is as follows...
    * Initialize library with `xorif_init()`
    * Specify component carrier configuration (e.g. `xorif_set_cc_num_rbs()`, `xorif_set_cc_numerology()`, etc.)
        * Note, during the specification phase, the validated inputs are stored in the s/w, they do not get written to the h/w until the "configure" step (below)
    * Configure the component carrier (i.e. `xorif_configure_cc()`)
        * The component carrier specification is validated to ensure it will fit in the hardware resources, and if successful the h/w register will be programmed appropriately
    * Enable the component carrier (i.e. `xorif_enable_cc()`)
    * Multiple component carriers can be specified and configured in the same manner
    * Close the library cleanly with `xorif_finish()`
* Other features of the library allow component carriers to disabled, re-configured, obtain stats, etc. See the API for details.
* The library also provides a register read/write interface (e.g. `xorif_read_fhi_reg()` and `xorif_write_fhi_reg()`)

### Example 1: Configure 1 component carrier (275 RBS, numerology 1)

~~~
// Precondition: libxorif has already been initialized with xorif_init()

// CC[0]: Set the number of RBs to 275
xorif_set_cc_num_rbs(0, 275);

// CC[0]: Set the numerology to 1 (normal cyclic prefix)
xorif_set_cc_numerology(0, 1, 0);

// CC[0]: Set the uplink timing parameters
// (delay compensation = 30us, control time advance = 90us, radio channel delay = 30us)
xorif_set_cc_ul_timing_parameters(0, 30, 90, 30);

// CC[0]: Set the downlink timing parameters
// (C-Plane delay compensation = 30us, U-Plane delay compensation = 30us, control time advance = 90us)
xorif_set_cc_dl_timing_parameters(0, 30, 30, 90);

// CC[0]: Set the down-link & up-link compression (9 bits, block-floating-point)
xorif_set_cc_dl_iq_compression(0, 9, 1);
xorif_set_cc_ul_iq_compression(0, 9, 1);

// CC[0]: Configure
xorif_configure_cc(0);

// CC[0]: Enable the component carrier
xorif_enable_cc(0)
~~~

### Example 2: Configure 2 component carriers (CC[0]: 275 RBS, numerology 1; CC[1]: 100 rbs, numerology 0)

~~~
// Precondition: libxorif has already been initialized with xorif_init()

// CC[0]: Set the number of RBs to 275
xorif_set_cc_num_rbs(0, 275);

// CC[0]: Set the numerology to 1 (normal cyclic prefix)
xorif_set_cc_numerology(0, 1, 0);

// CC[0]: Set the uplink timing parameters
// (delay compensation = 30us, control time advance = 90us, radio channel delay = 30us)
xorif_set_cc_ul_timing_parameters(0, 30, 90, 30);

// CC[0]: Set the downlink timing parameters
// (C-Plane delay compensation = 30us, U-Plane delay compensation = 30us, control time advance = 90us)
xorif_set_cc_dl_timing_parameters(0, 30, 30, 90);

// CC[0]: Set the down-link & up-link compression (9 bits, block-floating-point)
xorif_set_cc_dl_iq_compression(0, 9, 1);
xorif_set_cc_ul_iq_compression(0, 9, 1);

// CC[0]: Configure
xorif_configure_cc(0);

// CC[0]: Enable the component carrier
xorif_enable_cc(0)

// CC[1]: Set the number of RBs to 100
xorif_set_cc_num_rbs(1, 100);

// CC[1]: Set the numerology to 0 (normal cyclic prefix)
xorif_set_cc_numerology(1, 0, 0);

// CC[1]: Set the uplink timing parameters
// (delay compensation = 30us, control time advance = 90us, radio channel delay = 30us)
xorif_set_cc_ul_timing_parameters(1, 30, 90, 30);

// CC[1]: Set the downlink timing parameters
// (C-Plane delay compensation = 30us, U-Plane delay compensation = 30us, control time advance = 90us)
xorif_set_cc_dl_timing_parameters(1, 30, 30, 90);

// CC[1]: Set the down-link & up-link compression (9 bits, block-floating-point)
xorif_set_cc_dl_iq_compression(1, 9, 1);
xorif_set_cc_ul_iq_compression(1, 9, 1);

// CC[1]: Configure
xorif_configure_cc(1);

// CC[1]: Enable the component carrier
xorif_enable_cc(1)
~~~

### Example 3: Configure 1 component carrier (20 RBs, numerology 0, specifying sizes for max sections per symbol, max frames per symbol, protocol, etc.)

~~~
// Precondition: libxorif has already been initialized with xorif_init()

// CC[0]: Set the number of RBs to 20
xorif_set_cc_num_rbs(0, 20);

// CC[0]: Set the numerology to 1 (normal cyclic prefix)
xorif_set_cc_numerology(0, 0, 0);

// CC[0]: Set the uplink timing parameters
// (delay compensation = 30us, control time advance = 90us, radio channel delay = 30us)
xorif_set_cc_ul_timing_parameters(0, 30, 90, 30);

// CC[0]: Set the downlink timing parameters
// (C-Plane delay compensation = 30us, U-Plane delay compensation = 30us, control time advance = 90us)
xorif_set_cc_dl_timing_parameters(0, 30, 30, 90);

// CC[0]: Set the down-link & up-link compression (no compression)
xorif_set_cc_dl_iq_compression(0, 0, 0);
xorif_set_cc_ul_iq_compression(0, 0, 0);

// CC[0]: Set max number of sections per symbol (10 for both uplink and downlink)
// Note, this number affects the memory allocation for component carriers
xorif_set_cc_ul_sections_per_symbol(0, 10, 10);
xorif_set_cc_dl_sections_per_symbol(0, 10, 10);

// CC[0]: Set number of Ethernet frames per symbol (5 frames)
// Note, this number affects the memory allocation for component carriers
xorif_set_cc_frames_per_symbol(0, 5)

// CC[0]: Set protocol (ECPRI, no VLAN, Raw mode)
// Note, this sets up the default packet filter values
xorif_set_fhi_protocol(0, 0, 0);

// CC[0]: Set the eAxC ID values (4 DU bits, 1 BS bit, 3 CC bits, 8 RU bits)
xorif_set_fhi_eaxc_id(4, 1, 3, 8);

// CC[0]: Configure
xorif_configure_cc(0);

// CC[0]: Enable the component carrier
xorif_enable_cc(0)
~~~

## Debug Mode

* The library API command `xorif_debug()` can be used to enable library's "trace" feature
    * Level 0: No trace, but errors are still reported
    * Level 1: The trace feature shows main API calls and register read/writes
    * Level 2: The trace feature shows debug extra information in addition to the main API calls and register read/writes
* This can be a very useful feature for debugging or checking the component carrier configurations. For example...

~~~
XORIF> xorif_set_cc_num_rbs(0, 20)
XORIF> xorif_set_cc_numerology(0, 0, 0)
XORIF> xorif_set_cc_dl_iq_compression(0, 0, 0)
XORIF> xorif_set_cc_ul_iq_compression(0, 0, 0)
XORIF> xorif_set_cc_ul_sections_per_symbol(0, 10, 10)
XORIF> xorif_set_cc_dl_sections_per_symbol(0, 10, 10)
XORIF> xorif_configure_cc(0)
XORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x1 (1)
XORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_NUMRBS (0xE100)[0:8] <= 0x14 (20)
XORIF> WRITE_REG: ORAN_CC_NUMEROLOGY (0xE100)[16:18] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SYMPERSLOT (0xE100)[24:24] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_NUMRBS (0xE900)[0:8] <= 0x14 (20)
XORIF> WRITE_REG: ORAN_CC_SSB_NUMEROLOGY (0xE900)[16:18] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_SYMPERSLOT (0xE900)[24:24] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_SYM_NUM_INDEX (0xE114)[8:13] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_SYM_START_INDEX (0xE114)[0:5] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_CTRL_SYM_NUM_INDEX (0xE114)[16:21] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_UL_CTRL_SYM_NUM_INDEX (0xE114)[24:29] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_SSB_NUM_DATA_SYM_PER_CC (0xE914)[8:13] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_SSB_DATA_SYM_START_INDEX (0xE914)[0:5] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_NUM_SYM_PER_CC (0xE914)[16:21] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_DL_UD_IQ_WIDTH (0xE11C)[0:3] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_UD_COMP_METH (0xE11C)[4:7] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_MPLANE_UDCOMP_PARAM (0xE11C)[8:8] <= 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_UL_UD_IQ_WIDTH (0xE118)[0:3] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_UL_UD_COMP_METH (0xE118)[4:7] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_UL_MPLANE_UDCOMP_PARAM (0xE118)[8:8] <= 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_SSB_UD_IQ_WIDTH (0xE91C)[0:3] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_UD_COMP_METH (0xE91C)[4:7] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_MPLANE_UDCOMP_PARAM (0xE91C)[8:8] <= 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_NUM_CTRL_PER_SYMBOL_DL (0xE160)[0:15] <= 0xA (10)
XORIF> WRITE_REG: ORAN_CC_DL_CTRL_OFFSETS (0xE104)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_NUM_CTRL_PER_SYMBOL_UL (0xE164)[0:15] <= 0xA (10)
XORIF> WRITE_REG: ORAN_CC_UL_CTRL_OFFSETS (0xE10C)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_UL_BASE_OFFSET (0xE140)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC (0xE960)[0:15] <= 0x5 (5)
XORIF> WRITE_REG: ORAN_CC_SSB_CTRL_OFFSETS (0xE904)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE500)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE504)[0:15] <= 0x84 (132)
XORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED00)[0:15] <= 0x0 (0)
XORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED04)[0:15] <= 0x7F (127)
XORIF> WRITE_REG: ORAN_CC_MODVALS_DL (0xE168)[0:15] <= 0x14 (20)
XORIF> WRITE_REG: ORAN_CC_MODVALS_UL (0xE16C)[0:15] <= 0x14 (20)
XORIF> WRITE_REG: ORAN_CC_SSB_SECTS_X_SYMBOLS (0xE968)[0:15] <= 0xA (10)
XORIF> WRITE_REG: ORAN_CC_DL_SETUP_D_CYCLES (0xE138)[0:16] <= 0x33E5 (13285)
XORIF> WRITE_REG: ORAN_CC_DL_SETUP_C_CYCLES (0xE134)[0:16] <= 0x2562 (9570)
XORIF> WRITE_REG: ORAN_CC_DL_SETUP_C_ABS_SYMBOL (0xE130)[0:11] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_UL_SETUP_D_CYCLES (0xE128)[0:16] <= 0x37CD (14285)
XORIF> WRITE_REG: ORAN_CC_UL_SETUP_C_CYCLES (0xE124)[0:16] <= 0x11DA (4570)
XORIF> WRITE_REG: ORAN_CC_UL_SETUP_C_ABS_SYMBOL (0xE120)[0:11] <= 0x3 (3)
XORIF> WRITE_REG: ORAN_CC_SSB_SETUP_D_CYCLES (0xE938)[0:16] <= 0x33E5 (13285)
XORIF> WRITE_REG: ORAN_CC_SSB_SETUP_C_CYCLES (0xE934)[0:16] <= 0x2562 (9570)
XORIF> WRITE_REG: ORAN_CC_SSB_SETUP_C_ABS_SYMBOL (0xE930)[0:11] <= 0x2 (2)
XORIF> WRITE_REG: ORAN_CC_RELOAD (0xE000)[0:7] <= 0x1 (1)
XORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x1 (1)
XORIF> WRITE_REG: ORAN_CC_ENABLE (0xE004)[0:7] <= 0x1 (1)
~~~

* The register read/write debug information can be particularly useful. The debug trace shows...
    * The READ_REG or WRITE_REG operation
    * The symbolic name of the register
    * The address offset (in hexadecimal)
        * Note, the offset is relative to the base address of the device
    * The bit field, e.g. [0:15] means that the specified register field is located in bits 0 to 15
    * The value read or written in hexadecimal and decimal

## Additional Examples

* Configuring the protocol

~~~
XORIF> xorif_set_fhi_protocol(0, 0, 0)
XORIF> WRITE_REG: FRAM_PROTOCOL_DEFINITION (0x2200)[0:3] <= 0x0 (0)
XORIF> WRITE_REG: FRAM_GEN_VLAN_TAG (0x2200)[4:4] <= 0x0 (0)
XORIF> WRITE_REG: FRAM_SEL_IPV_ADDRESS_TYPE (0x2200)[5:6] <= 0x0 (0)
XORIF> xorif_set_fhi_packet_filter(0, ...)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6100) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6104) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6108) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x610C) <= 0xFFFFFEAE (4294966958)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_MASK_ADDR (0x6110) <= 0xCFFF (53247)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6120) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6124) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6128) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x612C) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_MASK_ADDR (0x6130) <= 0xFFFF (65535)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6140) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6144) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6148) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x614C) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_MASK_ADDR (0x6150) <= 0xFFFF (65535)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6160) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6164) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6168) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x616C) <= 0xFFFFFFFF (4294967295)
XORIF> WRITE_REG: DEFM_USER_DATA_FILTER_MASK_ADDR (0x6170) <= 0xFFFF (65535)
~~~

* Configuring the eAxC ID bits and RU ports (using "bit-mask" method)

~~~
XORIF> xorif_set_fhi_eaxc_id(4, 1, 3, 8)
XORIF> WRITE_REG: DEFM_CID_CC_SHIFT (0x6020)[0:3] <= 0x8 (8)
XORIF> WRITE_REG: DEFM_CID_CC_MASK (0x6024)[0:5] <= 0x7 (7)
XORIF> WRITE_REG: DEFM_CID_BS_SHIFT (0x6028)[0:3] <= 0xB (11)
XORIF> WRITE_REG: DEFM_CID_BS_MASK (0x602C)[0:5] <= 0x1 (1)
XORIF> WRITE_REG: DEFM_CID_DU_SHIFT (0x6030)[0:3] <= 0xC (12)
XORIF> WRITE_REG: DEFM_CID_DU_MASK (0x6034)[0:5] <= 0xF (15)
XORIF> DEBUG: eAxC ID bits: DDDDBCCCRRRRRRRR
XORIF> DEBUG: DU mask:      1111------------
XORIF> DEBUG: BS mask:      ----1-----------
XORIF> DEBUG: CC mask:      -----111--------
XORIF> DEBUG: RU mask:      --------11111111

XORIF> xorif_set_ru_ports(8, 5, 192, 0, 128, 64)
XORIF> WRITE_REG: DEFM_CID_SS_MASK (0x6038)[0:7] <= 0x1F (31)
XORIF> WRITE_REG: DEFM_CID_U_MASK (0x603C)[0:7] <= 0xC0 (192)
XORIF> WRITE_REG: DEFM_CID_U_VALUE (0x6040)[0:7] <= 0x0 (0)
XORIF> WRITE_REG: DEFM_CID_PRACH_MASK (0x6044)[0:7] <= 0xC0 (192)
XORIF> WRITE_REG: DEFM_CID_PRACH_VALUE (0x6048)[0:7] <= 0x80 (128)
XORIF> WRITE_REG: DEFM_CID_SSB_MASK (0x604C)[0:7] <= 0xC0 (192)
XORIF> WRITE_REG: DEFM_CID_SSB_VALUE (0x6050)[0:7] <= 0x40 (64)
XORIF> DEBUG: RU mask:     --------11111111
XORIF> DEBUG: SS mask:     -----------11111
XORIF> DEBUG: Mask:        --------11------
XORIF> DEBUG: User value:  --------00------
XORIF> DEBUG: PRACH value: --------10------
XORIF> DEBUG: SSB value:   --------01------
~~~

* Configuring the eAxC ID bits and RU ports (using "table" method)

~~~
# Set the number of bits for the eAxC IDs (du_bits = 4, bs_bits = 2, cc_bits = 3, ru_bits = 7)
XORIF> xorif_set_fhi_eaxc_id(4, 2, 3, 7)
XORIF> WRITE_REG: DEFM_CID_CC_SHIFT (0x6020)[0:3] <= 0x7 (7)
XORIF> WRITE_REG: DEFM_CID_CC_MASK (0x6024)[0:5] <= 0x7 (7)
XORIF> WRITE_REG: DEFM_CID_BS_SHIFT (0x6028)[0:3] <= 0xA (10)
XORIF> WRITE_REG: DEFM_CID_BS_MASK (0x602C)[0:5] <= 0x3 (3)
XORIF> WRITE_REG: DEFM_CID_DU_SHIFT (0x6030)[0:3] <= 0xC (12)
XORIF> WRITE_REG: DEFM_CID_DU_MASK (0x6034)[0:5] <= 0xF (15)
XORIF> DEBUG: eAxC ID bits: DDDDBBCCCRRRRRRR
XORIF> DEBUG: DU mask:      1111------------
XORIF> DEBUG: BS mask:      ----11----------
XORIF> DEBUG: CC mask:      ------111-------
XORIF> DEBUG: RU mask:      ---------1111111

# Clear the RU port mapping table
XORIF> xorif_clear_ru_ports_table()
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8003F000 (2147741696)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8003F001 (2147741697)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8003F002 (2147741698)
...
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8003F0FD (2147741949)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8003F0FE (2147741950)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8003F0FF (2147741951)

# Set the RU port mapping mode (mode 1 = 'direction + RU bits')
# Note, RU port mapping table address needs to be 8 bits or more (1 + 7)
XORIF> xorif_set_ru_ports_table_mode(1)
XORIF> WRITE_REG: DEFM_CID_MAP_MODE (0x6900)[0:1] <= 0x1 (1)

# Set RU port mapping for 8 x PUxCH streams, starting at address 0
# i.e. address 0 maps to PUxCH SS 0, address 1 maps to PUxCH SS 1, etc.
XORIF> xorif_set_ru_ports_table(0, 0, 1, 8)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80001000 (2147487744)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80041001 (2147749889)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80081002 (2148012034)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x800C1003 (2148274179)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80101004 (2148536324)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80141005 (2148798469)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80181006 (2149060614)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x801C1007 (2149322759)

# Set RU port mapping for 16 x PDxCH streams, starting at address 128
# i.e. address 128 maps to PDxCH SS 0, address 129 maps to PDxCH SS 1, etc.
XORIF> xorif_set_ru_ports_table(128, 0, 0, 16)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80000080 (2147483776)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80040081 (2147745921)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80080082 (2148008066)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x800C0083 (2148270211)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80100084 (2148532356)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80140085 (2148794501)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80180086 (2149056646)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x801C0087 (2149318791)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80200088 (2149580936)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80240089 (2149843081)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8028008A (2150105226)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x802C008B (2150367371)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8030008C (2150629516)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8034008D (2150891661)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x8038008E (2151153806)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x803C008F (2151415951)

# Set RU port mapping for 4 x SSB streams, starting at address 160
# i.e. address 160 maps to SSB SS 0, address 161 maps to SSB SS 1, etc.
XORIF> xorif_set_ru_ports_table(160, 0, 2, 4)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x800020A0 (2147492000)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x800420A1 (2147754145)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x800820A2 (2148016290)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x800C20A3 (2148278435)

# Set RU port mapping for 4 x PRACH streams, starting at address 32
# i.e. address 32 maps to PRACH SS 0, address 33 maps to PRACH SS 1, etc.
XORIF> xorif_set_ru_ports_table(32, 0, 4, 4)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80004020 (2147500064)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80044021 (2147762209)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x80084022 (2148024354)
XORIF> WRITE_REG: DEFM_CID_MAP_WR_STROBE_ADDR (0x6904) <= 0x800C4023 (2148286499)
~~~

The above programming would give rise to the following mapping table. Note, empty lines are not shown and should be unmapped. Use the xorif_clear_ru_ports_table() API call to clear the table prior to programming, or selectively remap modified addresses as required.

| eAxC RU Port | Address | Type   | Internal Port |
|--------------|---------|--------|---------------|
| 0            | 0       | PUxCH  | 0             |
| 1            | 1       | PUxCH  | 1             |
| 2            | 2       | PUxCH  | 2             |
| 3            | 3       | PUxCH  | 3             |
| 4            | 4       | PUxCH  | 4             |
| 5            | 5       | PUxCH  | 5             |
| 6            | 6       | PUxCH  | 6             |
| 7            | 7       | PUxCH  | 7             |
| 32           | 32      | PPRACH | 0             |
| 33           | 33      | PPRACH | 1             |
| 34           | 34      | PPRACH | 2             |
| 35           | 35      | PPRACH | 3             |
| 0            | 128     | PDxCH  | 0             |
| 1            | 129     | PDxCH  | 1             |
| 2            | 130     | PDxCH  | 2             |
| 3            | 131     | PDxCH  | 3             |
| 4            | 132     | PDxCH  | 4             |
| 5            | 133     | PDxCH  | 5             |
| 6            | 134     | PDxCH  | 6             |
| 7            | 135     | PDxCH  | 7             |
| ...          | ...     | ...    | ...           |
| 14           | 142     | PDxCH  | 14            |
| 15           | 143     | PDxCH  | 15            |
| 32           | 160     | SSB    | 0             |
| 33           | 161     | SSB    | 1             |
| 34           | 162     | SSB    | 2             |
| 35           | 163     | SSB    | 3             |

* Reading / writing registers

~~~
XORIF> xorif_read_fhi_reg(ORAN_CC_NUMEROLOGY)
XORIF> READ_REG: ORAN_CC_NUMEROLOGY (0xE100)[16:18] => 0x1 (1)

XORIF> xorif_read_fhi_reg(ORAN_CC_NUMRBS)
XORIF> READ_REG: ORAN_CC_NUMRBS (0xE100)[0:8] => 0x19 (25)
~~~

---

Copyright (C) 2019 - 2023 Advanced Micro Devices, Inc. All rights reserved.
