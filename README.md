# ORAN-Radio-Interface Project

## Project Information

This project contains the C LibMetal driver for the oran_radio_if IP. Additionally a C application is supplied demonstrating how to use the library to configure the IP. Full build instructions and scripts are provided for Vivado and Petalinux. 

## Programming references

|<div style="width:490px">Link</div>|Description|
|---|---|
|[Register_Map](https://www.xilinx.com/member/oran-radio-if.html)|This can be downloaded from the [**oran_radio_if lounge**](https://www.xilinx.com/member/oran-radio-if.html). The user guide, PG370 is also available for download. Registration required.|
|[Library_README](https://github.com/Xilinx/wireless-xorif/tree/master/src/libxorif)|Example of IP configuration using library calls. Includes full breakdown of IP register accesses. Note. this library contains C code that can be re-targeted for your application, if not using the library directly.|
|[Library_C_Code](https://github.com/Xilinx/wireless-xorif/blob/master/src/libxorif/xorif_fh_func.c)|Direct link to xorif library C function definitions. Use in conjunction with link above.|
|[Xorif_App_README](https://github.com/Xilinx/wireless-xorif/tree/master/src/xorif-app)|App overview and examples of App configuration files.|


## Directory Structure

This directory contains the following sub-directories:

~~~
├── bsp
├── doc
├── internal
├── output
├── scripts
├── src
├── xsa
└── yocto-recipes
~~~

* bsp
	* This directory is for Board Support Package (BSP) files generated by the project
* doc
	* This directory is for documentation, including auto-generated documentation (e.g. by Doxygen)
* internal
	* This directory is for internal tools, scripts, utilities, etc.
* output
	* This directory is for artefacts generated by the Vivado and PetaLinux flows 
* scripts
	* This directory is for top-level build scripts
* src
	* This directory is for source files for drivers, applications and scripts
* xsa
	* This directory is for hardware definition files (i.e. HDF or XSA)
* yocto-recipes
	* This directory is for recipe files for the yocto build system (used by PetaLinux to build the Linux and embedded software)

---

Copyright (C) 2019 - 2020  Xilinx, Inc.  All rights reserved.
