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
## 1st TCLARGS = board to use (zcu111|zcu102|zcu670)
## 2nd TCLARGS = Ip Configuration to select from the xil_vivado_configs.yml file (e4x8000x10_ss4x4_cc4x6600_dcc16x4x4x12x8_swtest|e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_cmw00i_aptw262144|...)
## 3rd TCLARGS = Command mode to run (impl)(exit)(nodate)(loop)
## 4rd TCLARGS = IP Directory        (Path to local IP repo.) [optional]

vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu111 -tclargs e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144  -tclargs impl
```

#### **Vivado TCL going deeper**
Also provided is `xil_vivado_utils.tcl` should you wish to augment or further explore the vivado design. Both supplied scripts
can be TCL "sourced" in your own flow and extended to achieve your end design.

### Build Everything
To run all the builds serially in a script, the following sequence can be called.
```console
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu670 -tclargs e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144        -tclargs implLoopNodateExit
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu111 -tclargs e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_cmw00i_aptw262144 -tclargs implLoopNodateExit
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu111 -tclargs e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi               -tclargs implLoopNodateExit
```

### PetaLinux build instructions

The Linux and embedded software is built using PetaLinux tools and a Makefile.

Build instructions:
1. Source the appropriate PetaLinux tools setting script, e.g. `source /proj/petalinux/2023.1/petalinux-v2023.1_daily_latest/tool/petalinux-v2023.1-final/settings.csh`
2. Copy the relevant hardware definition file (and rename if necessary) into `../xsa/<target>/system.xsa`
3. Run `make <target>`

Running `make` without a target specified displays some basic usage information, including the list of currently supported targets:

~~~
Usage  : make [target]
Example: make zcu670_e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144_exs

Supported targets:
    all
    clean_all
    zcu670_e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144_exs
    zcu111_e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144_exs

Notes:
    Board : [zcu102 | zcu111 | zcu670]
    Config: [e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144 = See content on YAML file for this entry.]
    Design: [exs = example system]
~~~

* Note 1: The "all" target builds all of the supported targets.
* Note 2: The "clean_all" target cleans all of the supported targets.
It uses "mrproper" which performs a deep-clean of the project and removes the `<plnx-proj-root>/build/`, `<plnx-proj-root>/images/` and `<TMPDIR>` directories.

The PetaLinux project directory is created as `../output/<target>_<tool version>/petalinux/`, e.g. `../output/zcu111_om5_exs_2021_1/petalinux/`.

The boot images are created inside the PetaLinux project directory at the usual location, i.e. `<plnx-proj-root>/images/linux/`. Typically, these are the BOOT.BIN, image.ub and boot.scr files. These files can be copied over onto an appropriately formatted SD card.

NOTE: In PetaLinux version 2021.1, the default build process generates an EXT4 based rootfs. The makefile creates a "wic" file which can be used to burn an SD-card with the boot partition (FAT32) and the rootfs partition (EXT4) using appropriate application software. However, in PetaLinux version 2021.2, the generated boot images have reverted back to using ramdisk based rootfs.

Consult PetaLinux user guide for details.

For more information on PetaLinux refer to:
* [UG1157](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1157-petalinux-tools-command-line-guide.pdf)
* [UG1144](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2019_2/ug1144-petalinux-tools-reference-guide.pdf)

Subsequent updates of BOOT.bin can be performed using the following command:
```console
petalinux-package --boot --fsbl images/linux/zynqmp_fsbl.elf --fpga images/linux/system.bit --pmufw images/linux/pmufw.elf --u-boot --force
```

### Full Sequence Examples
Build the Example System for the zcu670.
```console
## If you have a local repo
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu670 -tclargs e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144 -tclargs implLoopNodateExit -tclargs <optionalPathToIpRepo>
## To use the build
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu670 -tclargs e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144 -tclargs implLoopNodateExit 
mkdir ../xsa/zcu670_e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144_exs
cp ../output/zcu670_e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144_exs_2022_1/vivado/system.xsa ../xsa/zcu670_e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144_exs/system.xsa
make zcu670_e1x9000x25_ss8x4_cc4x6600_dcc16x4x4x16x16_aptw262144_exs
```
Build the Example System for the zcu670. This includes the OCP.
```console
vivado -mode tcl -source ./xil_vivado_build.tcl -tclargs zcu670 -tclargs e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi -tclargs implLoopNodateExit 
mkdir ../xsa/zcu670_e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi_exs
cp ../output/zcu670_e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi_exs_2022_1/vivado/system.xsa ../xsa/zcu670_e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi_exs/system.xsa
make zcu670_e1x9000x10_ss8x8_cc4x6600_dcc12x4x4x16x8_ocpi_exs
```

---

Copyright (C) 2019 - 2022  Advanced Micro Devices, Inc.  All rights reserved.
