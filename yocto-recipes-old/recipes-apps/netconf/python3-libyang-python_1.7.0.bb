SUMMARY = "Python CFFI bindings to libyang."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=2d1dc40dbc9e1af54d279b7e049a651f"

SRC_URI[md5sum] = "f29502ecdeb99ad25450de13d539bc34"
SRC_URI[sha256sum] = "d6c43bea66f3476be60884488694c1e04dfb1514f19c236ef424493c812ff539"

PYPI_PACKAGE = "libyang"

inherit pypi setuptools3

DEPENDS = "python3-cffi-native libyang"

RDEPENDS_${PN} += "python3-cffi libyang"

BBCLASSEXTEND = "native nativesdk" 