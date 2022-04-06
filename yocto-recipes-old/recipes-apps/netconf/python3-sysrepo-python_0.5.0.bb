SUMMARY = "Python CFFI bindings to sysrepo."
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=fcef73ae39ec9937de8fcc129e74afb2"

SRC_URI[md5sum] = "21d8a7b8721db538fe90c13bfc299196"
SRC_URI[sha256sum] = "4279e7420df1de5063dc75fc33a784278a5308cec8243468b1db5c9194c0882c"

PYPI_PACKAGE = "sysrepo"

inherit pypi setuptools3

DEPENDS = "python3-cffi-native libyang sysrepo"

RDEPENDS_${PN} += "python3-cffi libyang sysrepo"

BBCLASSEXTEND = "native nativesdk" 