# LIBXORIF

## LIBXORIF Source Directory

* This directory contains the source files and Makefile for the ORAN-Radio-Interface C API Library (libxorif)

### API

* The C API Library API is documented [here](../../doc/html/group__libxorif-api.html)

* The example application (xorif-app) demonstrates the use of the library API, and is a good starting point for reference

### Building

* The libxorif library is built as part of the PetaLinux build system, and can be found in the `/usr/lib/` directory of the target Linux installation.

* It can also be built separately using an appropriate SDK including the cross-compilation tools and dependencies (e.g. libmetal library).

    * Run: `make`
	
### Usage

* The basic use of the library API is as follows...
	* Initialize library (i.e. `xorif_init()`)
	* Specify component carrier configuration (e.g. `xorif_set_cc_num_rbs()`, `xorif_set_cc_numerology()`, etc.)
		* Note, during the specification phase, the validated inputs are stored in the s/w, they do not get written to the h/w until the "configure" step (below)
	* Configure the component carrier (i.e. `xorif_configure_cc()`)
		* The component carrier specification is validated to ensure it will fit in the hardware resources, and if successful the h/w register will be programmed appropriately
		* The final step of the "configure" is to reload and enable the specified component carrier
	* Multiple component carriers can be specified and configured, and this should always be done lowest CC id number first, i.e. CC[0], CC[1], CC[2], etc.
* Other features of the library allow component carriers to disabled, re-configured, obtain stats, etc. See the API for details.
* The library also provides a register read/write interface (e.g. `xorif_read_fhi_reg()` and `xorif_write_fhi_reg()`)

### Example 1: Configure 1 component carrier (275 RBS, numerology 1)

~~~
	// Initialize the library
	xorif_init(NULL, NULL);
	
	// CC[0]: Set the number of RBs to 275
	xorif_set_cc_num_rbs(0, 275);
	
	// CC[0]: Set the numerology to 1 (normal cyclic prefix)
	xorif_set_cc_numerology(0, 1, 0);
	
	// CC[0]: Set the timing advances & deskew (deskew = 30us, up-link advance = 90us, down-link = 90us)
	xorif_set_cc_time_advance(0, 30, 90, 90);
	
	// CC[0]: Set the down-link compression (9 bits, block-floating-point)
	xorif_set_cc_dl_iq_compression(0, 9, 1);
	xorif_set_cc_ul_iq_compression(0, 9, 1);
	
	// CC[0]: Configure
	xorif_configure_cc(0);
~~~

### Example 2: Configure 2 component carriers (CC[0]: 275 RBS, numerology 1; CC[1]: 100 rbs, numerology 0)

~~~
	// Initialize the library
	xorif_init(NULL, NULL);
	
	// Component carrier 0 ...
	// CC[0]: Set the number of RBs to 275
	xorif_set_cc_num_rbs(0, 275);
	
	// CC[0]: Set the numerology to 1 (normal cyclic prefix)
	xorif_set_cc_numerology(0, 1, 0);
	
	// CC[0]: Set the timing advances & deskew (deskew = 30us, up-link advance = 90us, down-link = 90us)
	xorif_set_cc_time_advance(0, 30, 90, 90);
	
	// CC[0]: Set the down-link compression (9 bits, block-floating-point)
	xorif_set_cc_dl_iq_compression(0, 9, 1);
	xorif_set_cc_ul_iq_compression(0, 9, 1);
	
	// CC[0]: Configure
	xorif_configure_cc(0);
	
	// Component carrier 1 ...
	// CC[1]: Set the number of RBs to 100
	xorif_set_cc_num_rbs(1, 100);
	
	// CC[1]: Set the numerology to 0 (normal cyclic prefix)
	xorif_set_cc_numerology(1, 0, 0);
	
	// CC[1]: Set the timing advances & deskew (deskew = 30us, up-link advance = 90us, down-link = 90us)
	xorif_set_cc_time_advance(1, 30, 90, 90);
	
	// CC[1]: Set the down-link compression (9 bits, block-floating-point)
	xorif_set_cc_dl_iq_compression(1, 9, 1);
	xorif_set_cc_ul_iq_compression(1, 9, 1);
	
	// CC[1]: Configure
	xorif_configure_cc(1);
~~~

## Debug Mode

* The library API command `xorif_debug(1)` can be used to enable library "trace" feature
	* The trace feature shows API calls, register read/writes and other useful debug information
* This can be a very useful feature for debugging or checking the component carrier configurations.For example...

~~~
	LIBXORIF> DEBUG: xorif_init(NULL, NULL)
	LIBXORIF> DEBUG: Warning! Libmetal framework is already running.
	LIBXORIF> DEBUG: xorif_set_cc_num_rbs(0, 275)
	LIBXORIF> DEBUG: xorif_set_cc_numerology(0, 1, 0)
	LIBXORIF> DEBUG: xorif_set_cc_time_advance(0, 30, 90, 90)
	LIBXORIF> DEBUG: READ_REG: CFG_CONFIG_XRAN_MAX_CC [0x2c] => 0x4 (4)
	LIBXORIF> DEBUG: xorif_set_cc_dl_iq_compression(0, 9, 1)
	LIBXORIF> DEBUG: READ_REG: CFG_CONFIG_XRAN_MAX_CC [0x2c] => 0x4 (4)
	LIBXORIF> DEBUG: xorif_set_cc_ul_iq_compression(0, 9, 1)
	LIBXORIF> DEBUG: READ_REG: CFG_CONFIG_XRAN_MAX_CC [0x2c] => 0x4 (4)
	LIBXORIF> DEBUG: xorif_configure_cc(0)
	LIBXORIF> DEBUG: READ_REG: CFG_CONFIG_XRAN_MAX_CC [0x2c] => 0x4 (4)
	LIBXORIF> DEBUG: READ_REG: CFG_CONFIG_XRAN_MAX_CTRL_SYMBOLS [0x4c] => 0xd (13)
	LIBXORIF> DEBUG: READ_REG: CFG_CONFIG_XRAN_MAX_DL_SYMBOLS [0x30] => 0x6 (6)
	LIBXORIF> DEBUG: READ_REG: ORAN_CC_ENABLE [0xe004] => 0x3 (3)
	LIBXORIF> DEBUG: READ_REG: ORAN_CC_ENABLE [0xe004] => 0x3 (3)
	LIBXORIF> DEBUG: xorif_fhi_get_cc_alloc(1, ...)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_NUMRBS [0xe100 + 0x70] => 0x64 (100)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_UL_CTRL_SYM_NUM_INDEX [0xe114 + 0x70] => 0x2 (2)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_UL_CTRL_OFFSETS [0xe10c + 0x70] => 0x44c (1100)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_UL_CTRL_UNROLLED_OFFSETS [0xe110 + 0x70] => 0x113 (275)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_DL_CTRL_SYM_NUM_INDEX [0xe114 + 0x70] => 0x2 (2)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_DL_CTRL_OFFSETS [0xe104 + 0x70] => 0x44c (1100)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_DL_CTRL_UNROLLED_OFFSETS [0xe108 + 0x70] => 0x113 (275)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_DL_DATA_SYM_NUM_INDEX [0xe114 + 0x70] => 0x2 (2)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_DL_DATA_SYM_START_INDEX [0xe114 + 0x70] => 0x2 (2)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_DL_UD_COMP_METH [0xe11c + 0x70] => 0x1 (1)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_DL_UD_IQ_WIDTH [0xe11c + 0x70] => 0x9 (9)
	LIBXORIF> DEBUG: READ_REG_OFFSET: ORAN_CC_DL_DATA_UNROLL_OFFSET [0xe500 + 0x8] => 0x898 (2200)
	LIBXORIF> DEBUG: Configuration valid
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_NUMEROLOGY [0xe100 + 0x0] <= 0x1 (1)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_NUMRBS [0xe100 + 0x0] <= 0x113 (275)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_SYMPERSLOT [0xe100 + 0x0] <= 0x0 (0)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_DATA_SYM_NUM_INDEX [0xe114 + 0x0] <= 0x2 (2)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_DATA_SYM_START_INDEX [0xe114 + 0x0] <= 0x0 (0)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_CTRL_SYM_NUM_INDEX [0xe114 + 0x0] <= 0x4 (4)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_UL_CTRL_SYM_NUM_INDEX [0xe114 + 0x0] <= 0x4 (4)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_UD_IQ_WIDTH [0xe11c + 0x0] <= 0x9 (9)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_UD_COMP_METH [0xe11c + 0x0] <= 0x1 (1)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_UL_UD_IQ_WIDTH [0xe118 + 0x0] <= 0x9 (9)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_UL_UD_COMP_METH [0xe118 + 0x0] <= 0x1 (1)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_CTRL_OFFSETS [0xe104 + 0x0] <= 0x0 (0)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_CTRL_UNROLLED_OFFSETS [0xe108 + 0x0] <= 0x0 (0)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_UL_CTRL_OFFSETS [0xe10c + 0x0] <= 0x0 (0)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_UL_CTRL_UNROLLED_OFFSETS [0xe110 + 0x0] <= 0x0 (0)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_DATA_UNROLL_OFFSET [0xe500 + 0x0] <= 0x0 (0)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_DATA_UNROLL_OFFSET [0xe500 + 0x4] <= 0x44c (1100)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_MAX_SYMBOLS [0xe158 + 0x0] <= 0x118 (280)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_MODVALS_DL [0xe168 + 0x0] <= 0x80 (128)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_MODVALS_UL [0xe16c + 0x0] <= 0x100 (256)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_SETUP_D_CYCLES [0xe138 + 0x0] <= 0x2ffd (12285)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_SETUP_C_CYCLES [0xe134 + 0x0] <= 0x2ffd (12285)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_DL_SETUP_C_ABS_SYMBOL [0xe130 + 0x0] <= 0x1 (1)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_UL_SETUP_D_CYCLES [0xe128 + 0x0] <= 0x37cd (14285)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_UL_SETUP_C_CYCLES [0xe124 + 0x0] <= 0x8ed (2285)
	LIBXORIF> DEBUG: WRITE_REG_OFFSET: ORAN_CC_UL_SETUP_C_ABS_SYMBOL [0xe120 + 0x0] <= 0x2 (2)
	LIBXORIF> DEBUG: WRITE_REG: ORAN_CC_RELOAD [0xe000] <= 0x1 (1)
	LIBXORIF> DEBUG: READ_REG: ORAN_CC_ENABLE [0xe004] => 0x3 (3)
	LIBXORIF> DEBUG: WRITE_REG: ORAN_CC_ENABLE [0xe004] <= 0x3 (3)
~~~

---

Copyright (C) 2019 - 2020  Xilinx, Inc.  All rights reserved.
