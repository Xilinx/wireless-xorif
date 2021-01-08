# ORAN-Radio-Interface Project

## Scripts Directory

This directory is for top-level build scripts.

### Vivado build instructions

Usage: 
```console
vivado -mode tcl -source ./xil_vivado_build.tcl
```
Once loaded, review the printed help. When you are familiar with the flow and want to fully script the build, you can call vivado with launch arguments.
```console
## This style uses a set of strings concatenated together to tell the 
## script what mode it should built in. Note. case is ignored.
## 
## 1st TCLARGS = board to use (zcu111|zcu102)
## 2nd TCLARGS = Ip Mode to select (om5)
## 3rd TCLARGS = Command mode to run (impl)(exit)(nodate)
## 4rd TCLARGS = IP Directory        (Path to local IP repo.) [optional]

vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu102 -tclargs om5  -tclargs impl
```

#### **Vivado TCL going deeper**
Also provided is `xil_vivado_utils.tcl` should you wish to augment or further explore the vivado design. Both supplied scripts
can be TCL "sourced" in your own flow and extended to achieve your end design.

### Build Everything
To run all the builds serially in a script, the following sequence can be called.
```console
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu102 -tclargs om5        -tclargs implNodateExit
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu111 -tclargs om5        -tclargs implNodateExit
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu111 -tclargs om5_25     -tclargs implNodateExit
```

### PetaLinux build instructions

The Linux and embedded software is built using PetaLinux tools and a Makefile.

Build instructions:
1. Source the appropriate PetaLinux tools setting script, e.g. `source /proj/petalinux/2020.1/petalinux-v2020.1_daily_latest/tool/petalinux-v2020.1-final/settings.csh`
2. Copy the relevant hardware definition file (and rename if necessary) into `../xsa/<target>/system.xsa`
3. Run `make <target>`

Running `make` without a target specified displays some basic usage information, including the list of currently supported targets:

~~~
Usage: make [target]
Example: make zcu111_om5_exs

Supported targets:
    all
    clean_all
    zcu102_om5_exs
    zcu111_om5_exs

Notes:
    Board: [zcu102 | zcu111]
    Mode: [om5 = ORAN]
    Design: [exs = example system]
~~~

* Note 1: The "all" target builds all of the supported targets.
* Note 2: The "clean_all" target cleans all of the supported targets.
It uses "mrproper" which performs a deep-clean of the project and removes the `<plnx-proj-root>/build/`, `<plnx-proj-root>/images/` and `<TMPDIR>` directories.

The PetaLinux project directory is created as `../output/<target>_<tool version>/petalinux/`, e.g. `../output/zcu111_om5_exs_2020_1/petalinux/`.

The boot images (typically BOOT.bin and image.ub) are created inside the PetaLinux project directory at the usual location, i.e. `<plnx-proj-root>/images/linux/`

For more information on PetaLinux refer to:
* [UG1157](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1157-petalinux-tools-command-line-guide.pdf)
* [UG1144](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1144-petalinux-tools-reference-guide.pdf)

Subsequent updates of BOOT.bin can be performed using the following command:
```console
petalinux-package --boot --fsbl images/linux/zynqmp_fsbl.elf --fpga images/linux/system.bit --pmufw images/linux/pmufw.elf --u-boot --force
```

---

Copyright (C) 2019 - 2020  Xilinx, Inc.  All rights reserved.
