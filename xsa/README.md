# ORAN-Radio-Interface Project

## XSA Directory

This directory is for hardware definition files (i.e. HDF or XSA).

The hardware definition files (usually an XSA file) is generated as part of the Vivado flow. However, the build process expects to locate the XSA in specific subdirectories according to the build target and tool version.

For example:
~~~
├── zcu102_om5_2020_1
│   └── system.xsa
├── zcu111_om5_2020_1
│   └── system.xsa
├── zcu111_om5_2020_2
│   └── system.xsa
└─── zcu111_om5_ptp_2020_2
    └── system.xsa
~~~

See [here](../scripts/README.md) for more details of the build process.

---

Copyright (C) 2019 - 2021  Xilinx, Inc.  All rights reserved.
