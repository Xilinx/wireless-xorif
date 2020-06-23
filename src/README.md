# ORAN-Radio-Interface Project

## Source Directory

### libxorif

* This directory contains the source files and Makefile for the ORAN-Radio-Interface C API Library

### xlnx-ptp-timer

* This directory contains the source files and Makefiles for the example PTP timer driver

### xorif-app

* This directory contains the source files and Makefiles for the ORAN-Radio-Interface Example Application

### xorif-scripts

* This directory contains various scripts for configuring and using the ORAN-Radio-Interface IP

### xorif-startup

* This directory contains the start-up scripts for configuring and using the ORAN-Radio-Interface IP
* The start-up scripts are executed automatically during the boot process

### xorif-test

* This directory contains the source files and Makefiles for ORAN-Radio-Interface Test Application
* The test application performs some basic unit tests on the ORAN-Radio-Interface IP and C API library
	- Register read/writes
	- Library calls
* Note, many of the unit tests performed by the test application require specific configuration of the IP (e.g. the size of allocated buffers), and as such will not run successfully on other configurations

---

Copyright (C) 2019 - 2020  Xilinx, Inc.  All rights reserved.
