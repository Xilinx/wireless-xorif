SUMMARY = "YANG data modelling language parser and toolkit"
SECTION = "libs"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=2982272c97a8e417a844857ca0d303b1"

SRC_URI = "https://github.com/CESNET/libyang/archive/refs/tags/v${PV}.zip"
SRC_URI[md5sum] = "55036590611a0576eb75e0eb907fc037"
SRC_URI[sha256sum] = "ba2abf3585b10eaeec9411e922a354ae751137843509c2f7ea51875b65275da8"

S = "${WORKDIR}/${PN}-${PV}"

DEPENDS = "libpcre"

FILES_${PN} += "/usr/lib/libyang1/*"

inherit cmake pkgconfig

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
EXTRA_OECMAKE = " -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE:String=Release "

BBCLASSEXTEND = "native nativesdk"