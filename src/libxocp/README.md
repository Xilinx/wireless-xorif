# Xilinx O-RAN Radio PXXCH Channel Processor (LIBXOCP)

* The C library (libxocp) is a Linux shared library that provides the functions needed to access, configure and manage the O-RAN Radio PXXCH Channel Processor IP core.

* Release 2023.1 has the option of the ORAN Channel Processor (OCP) integrated with ORAN Radio Interface (ORIF), which uses common s/w driver library (libxorif), but with separate APIs.

## API

* The C library API is documented [here](../../doc/libxocp_html/group__API.html)

    * The main header for the ORAN Channel Processor (OCP) is `xocp_api.h`

## Building

* The libxocp library is built as part of the PetaLinux build system, and can be found in the `/usr/lib/` directory of the target Linux installation

* It can also be built separately using an appropriate SDK including the cross-compilation tools and dependencies (e.g. libmetal library)

    * Run: `make`

## Python Bindings

* The Python module `pylibxocp.py` provides (Python 3) bindings for the C library
    * The `pylibxocp.py` module can be found in the current directory, and it is also installed as part of the PetaLinux build, and can be found in the `/usr/share/xorif` directory
    * The Python bindings use CFFI and can be used directly, or in conjunction with other libraries such as Pyro4 which allows remote procedure calls to Python objects
    * The Pyro4 and CFFI libraries are included as part of the Petalinux build, or can be installed manually with `pip install`
    * The Python bindings are provided for "ease of use". The Python API is largely the same as the C API, but more "Pythonic", e.g. allowing "dicts" to be passed in rather than pointers to structures.