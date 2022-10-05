# ORAN-Radio-Interface Project

## IP Documentation
This can be found on the [**oran_radio_if lounge**](https://www.xilinx.com/member/oran-radio-if.html). Registration is required for this site.

## Project Information

This project contains the C LibMetal driver for the oran_radio_if IP. Additionally a C application is supplied demonstrating how to use the library to configure the IP. Full build instructions and scripts are provided for Vivado and Petalinux. 

## Programming References

|Source|Description|
|---|---|
|[Register Map](https://www.xilinx.com/member/oran-radio-if.html)|This can be downloaded from the [**oran_radio_if lounge**](https://www.xilinx.com/member/oran-radio-if.html). The user guide, [**PG370**](https://www.xilinx.com/member/oran-radio-if.html) is also available for download. Registration required.|
|[SW Driver Library README](https://github.com/Xilinx/wireless-xorif/tree/master/src/libxorif/README.md)|Example of IP configuration using library calls. Includes full breakdown of IP register accesses. Note. this library contains C code that can be re-targeted for your application, if not using the library directly.|
|[XORIF-APP README](https://github.com/Xilinx/wireless-xorif/tree/master/src/xorif-app/README.md)|App overview and examples of App configuration files.|

## GitHub Usage
The master branch of the GitHub repository contains the latest version for the current Xilinx release.

Releases are aligned with the Xilinx tools release schedule, at which point the archive is tagged e.g., v2021.2, and a "Release" is created on GitHub. All post-release fixes/changes are made on the master/HEAD.

Therefore, the "latest" usable code for the current Xilinx release is normally master/HEAD.

Code for previous Xilinx releases can of course be checked-out via commit hash, or by tag.

## Directory Structure

This directory contains the following sub-directories:

~~~
├── doc
├── internal
├── output
├── scripts
├── src
├── xsa
└── yocto-recipes
~~~

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

Copyright (C) 2019 - 2022 Advanced Micro Devices, Inc. All rights reserved.
