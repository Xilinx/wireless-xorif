# ORAN-Radio-Interface Project

## Doc Directory

This directory is for documentation, including auto-generated documentation (e.g. by Doxygen).

### Doxygen instructions

Documentation for the embedded software documentation can be generated automatically using Doxygen.

Separate documentation is available for the driver libraries and the example application software.

The documentation can be (re-)generated using the Makefile provided in this directory, e.g.
```
make # Generate all documentation
make libxorif # Generate documentation for the libxorif device driver library
make xorif-app # Generate documentation for the example application software
make clean # Deletes all the generated documentation directories
```

Doxygen generates the documentation in the `<doc>_html/` directory (e.g. `libxorif_html/`) and the main file is `<doc>_html/index.html` which can be opened with a standard web browser.

For more information on Doxygen refer to:
* [Doxygen](http://www.doxygen.nl/)

---

Copyright (C) 2019 - 2022  Xilinx, Inc.  All rights reserved.
