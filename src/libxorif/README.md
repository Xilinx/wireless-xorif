# LIBXORIF

## LIBXORIF Source Directory

* This directory contains the source files and Makefile for the ORAN-Radio-Interface Front-Haul Interface C API Library (libxorif)

### API

* The C API Library API is documented [here](../../doc/html/group__libxorif-api.html)

* The example application (xorif-app) demonstrates the use of the library API, and is a good starting point for reference

### Building

* The libxorif library is built as part of the PetaLinux build system, and can be found in the `/usr/lib/` directory of the target Linux installation.

* It can also be built separately using an appropriate SDK including the cross-compilation tools and dependencies (e.g. libmetal library).

    * Run: `make`
	
### Usage

* The basic use of the library API is as follows...
	* Initialize library with `xorif_init()`
	* Specify component carrier configuration (e.g. `xorif_set_cc_num_rbs()`, `xorif_set_cc_numerology()`, etc.)
		* Note, during the specification phase, the validated inputs are stored in the s/w, they do not get written to the h/w until the "configure" step (below)
	* Configure the component carrier (i.e. `xorif_configure_cc()`)
		* The component carrier specification is validated to ensure it will fit in the hardware resources, and if successful the h/w register will be programmed appropriately
    * Enable the component carrier (i.e. `xorif_enable_cc()`)
	* Multiple component carriers can be specified and configured, and this should always be done lowest CC id number first, i.e. CC[0], CC[1], CC[2], etc.
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
LIBXORIF> xorif_set_cc_num_rbs(0, 20)
LIBXORIF> xorif_set_cc_numerology(0, 0, 0)
LIBXORIF> xorif_set_cc_dl_iq_compression(0, 0, 0)
LIBXORIF> xorif_set_cc_ul_iq_compression(0, 0, 0)
LIBXORIF> xorif_set_cc_ul_sections_per_symbol(0, 10, 10)
LIBXORIF> xorif_set_cc_dl_sections_per_symbol(0, 10, 10)
LIBXORIF> xorif_configure_cc(0)
LIBXORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x1 (1)
LIBXORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_NUMRBS (0xE100)[0:8] <= 0x14 (20)
LIBXORIF> WRITE_REG: ORAN_CC_NUMEROLOGY (0xE100)[16:18] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SYMPERSLOT (0xE100)[24:24] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_NUMRBS (0xE900)[0:8] <= 0x14 (20)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_NUMEROLOGY (0xE900)[16:18] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SYMPERSLOT (0xE900)[24:24] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_SYM_NUM_INDEX (0xE114)[8:13] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_SYM_START_INDEX (0xE114)[0:5] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_CTRL_SYM_NUM_INDEX (0xE114)[16:21] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_UL_CTRL_SYM_NUM_INDEX (0xE114)[24:29] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_NUM_DATA_SYM_PER_CC (0xE914)[8:13] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_DATA_SYM_START_INDEX (0xE914)[0:5] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_NUM_SYM_PER_CC (0xE914)[16:21] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_DL_UD_IQ_WIDTH (0xE11C)[0:3] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_UD_COMP_METH (0xE11C)[4:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_MPLANE_UDCOMP_PARAM (0xE11C)[8:8] <= 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_UL_UD_IQ_WIDTH (0xE118)[0:3] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_UL_UD_COMP_METH (0xE118)[4:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_UL_MPLANE_UDCOMP_PARAM (0xE118)[8:8] <= 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_UD_IQ_WIDTH (0xE91C)[0:3] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_UD_COMP_METH (0xE91C)[4:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_MPLANE_UDCOMP_PARAM (0xE91C)[8:8] <= 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_NUM_CTRL_PER_SYMBOL_DL (0xE160)[0:15] <= 0xA (10)
LIBXORIF> WRITE_REG: ORAN_CC_DL_CTRL_OFFSETS (0xE104)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_NUM_CTRL_PER_SYMBOL_UL (0xE164)[0:15] <= 0xA (10)
LIBXORIF> WRITE_REG: ORAN_CC_UL_CTRL_OFFSETS (0xE10C)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_UL_BASE_OFFSET (0xE140)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_NUMSSBCTRLSECT_X_SYM_X_CC (0xE960)[0:15] <= 0x5 (5)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_CTRL_OFFSETS (0xE904)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE500)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_DL_DATA_UNROLL_OFFSET (0xE504)[0:15] <= 0x84 (132)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED00)[0:15] <= 0x0 (0)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_DATA_UNROLL_OFFSET (0xED04)[0:15] <= 0x7F (127)
LIBXORIF> WRITE_REG: ORAN_CC_MODVALS_DL (0xE168)[0:15] <= 0x14 (20)
LIBXORIF> WRITE_REG: ORAN_CC_MODVALS_UL (0xE16C)[0:15] <= 0x14 (20)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SECTS_X_SYMBOLS (0xE968)[0:15] <= 0xA (10)
LIBXORIF> WRITE_REG: ORAN_CC_DL_SETUP_D_CYCLES (0xE138)[0:16] <= 0x33E5 (13285)
LIBXORIF> WRITE_REG: ORAN_CC_DL_SETUP_C_CYCLES (0xE134)[0:16] <= 0x2562 (9570)
LIBXORIF> WRITE_REG: ORAN_CC_DL_SETUP_C_ABS_SYMBOL (0xE130)[0:11] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_UL_SETUP_D_CYCLES (0xE128)[0:16] <= 0x37CD (14285)
LIBXORIF> WRITE_REG: ORAN_CC_UL_SETUP_C_CYCLES (0xE124)[0:16] <= 0x11DA (4570)
LIBXORIF> WRITE_REG: ORAN_CC_UL_SETUP_C_ABS_SYMBOL (0xE120)[0:11] <= 0x3 (3)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SETUP_D_CYCLES (0xE938)[0:16] <= 0x33E5 (13285)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SETUP_C_CYCLES (0xE934)[0:16] <= 0x2562 (9570)
LIBXORIF> WRITE_REG: ORAN_CC_SSB_SETUP_C_ABS_SYMBOL (0xE930)[0:11] <= 0x2 (2)
LIBXORIF> WRITE_REG: ORAN_CC_RELOAD (0xE000)[0:7] <= 0x1 (1)
LIBXORIF> READ_REG: ORAN_CC_ENABLE (0xE004)[0:7] => 0x1 (1)
LIBXORIF> WRITE_REG: ORAN_CC_ENABLE (0xE004)[0:7] <= 0x1 (1)
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
LIBXORIF> xorif_set_fhi_protocol(0, 0, 0)
LIBXORIF> WRITE_REG: FRAM_PROTOCOL_DEFINITION (0x2200)[0:3] <= 0x0 (0)
LIBXORIF> WRITE_REG: FRAM_GEN_VLAN_TAG (0x2200)[4:4] <= 0x0 (0)
LIBXORIF> WRITE_REG: FRAM_SEL_IPV_ADDRESS_TYPE (0x2200)[5:6] <= 0x0 (0)
LIBXORIF> xorif_set_fhi_packet_filter(0, ...)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6100) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6104) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6108) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x610C) <= 0xFFFFFEAE (4294966958)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_MASK_ADDR (0x6110) <= 0xCFFF (53247)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6120) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6124) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6128) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x612C) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_MASK_ADDR (0x6130) <= 0xFFFF (65535)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6140) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6144) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6148) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x614C) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_MASK_ADDR (0x6150) <= 0xFFFF (65535)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6160) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6164) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x6168) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_WORD_ADDR (0x616C) <= 0xFFFFFFFF (4294967295)
LIBXORIF> WRITE_REG: DEFM_USER_DATA_FILTER_MASK_ADDR (0x6170) <= 0xFFFF (65535)
~~~

* Configuring the eXaC ID bits and RU ports

~~~
LIBXORIF> xorif_set_fhi_eaxc_id(4, 1, 3, 8)
LIBXORIF> WRITE_REG: DEFM_CID_CC_SHIFT (0x6020)[0:3] <= 0x8 (8)
LIBXORIF> WRITE_REG: DEFM_CID_CC_MASK (0x6024)[0:5] <= 0x7 (7)
LIBXORIF> WRITE_REG: DEFM_CID_BS_SHIFT (0x6028)[0:3] <= 0xB (11)
LIBXORIF> WRITE_REG: DEFM_CID_BS_MASK (0x602C)[0:5] <= 0x1 (1)
LIBXORIF> WRITE_REG: DEFM_CID_DU_SHIFT (0x6030)[0:3] <= 0xC (12)
LIBXORIF> WRITE_REG: DEFM_CID_DU_MASK (0x6034)[0:5] <= 0xF (15)
LIBXORIF> DEBUG: eAxC ID bits: DDDDBCCCRRRRRRRR
LIBXORIF> DEBUG: DU mask:      1111------------
LIBXORIF> DEBUG: BS mask:      ----1-----------
LIBXORIF> DEBUG: CC mask:      -----111--------
LIBXORIF> DEBUG: RU mask:      --------11111111

LIBXORIF> xorif_set_ru_ports(8, 5, 192, 0, 128, 64)
LIBXORIF> WRITE_REG: DEFM_CID_SS_MASK (0x6038)[0:7] <= 0x1F (31)
LIBXORIF> WRITE_REG: DEFM_CID_U_MASK (0x603C)[0:7] <= 0xC0 (192)
LIBXORIF> WRITE_REG: DEFM_CID_U_VALUE (0x6040)[0:7] <= 0x0 (0)
LIBXORIF> WRITE_REG: DEFM_CID_PRACH_MASK (0x6044)[0:7] <= 0xC0 (192)
LIBXORIF> WRITE_REG: DEFM_CID_PRACH_VALUE (0x6048)[0:7] <= 0x80 (128)
LIBXORIF> WRITE_REG: DEFM_CID_SSB_MASK (0x604C)[0:7] <= 0xC0 (192)
LIBXORIF> WRITE_REG: DEFM_CID_SSB_VALUE (0x6050)[0:7] <= 0x40 (64)
LIBXORIF> DEBUG: RU mask:     --------11111111
LIBXORIF> DEBUG: SS mask:     -----------11111
LIBXORIF> DEBUG: Mask:        --------11------
LIBXORIF> DEBUG: User value:  --------00------
LIBXORIF> DEBUG: PRACH value: --------10------
LIBXORIF> DEBUG: SSB value:   --------01------
~~~

* Reading / writing registers

~~~
LIBXORIF> xorif_read_fhi_reg(ORAN_CC_NUMEROLOGY)
LIBXORIF> READ_REG: ORAN_CC_NUMEROLOGY (0xE100)[16:18] => 0x1 (1)

LIBXORIF> xorif_read_fhi_reg(ORAN_CC_NUMRBS)
LIBXORIF> READ_REG: ORAN_CC_NUMRBS (0xE100)[0:8] => 0x19 (25)
~~~

---

Copyright (C) 2019 - 2021  Xilinx, Inc.  All rights reserved.
