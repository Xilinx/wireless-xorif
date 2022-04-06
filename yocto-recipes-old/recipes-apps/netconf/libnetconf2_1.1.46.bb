SUMMARY = "libnetconf2 is a NETCONF library in C intended for building NETCONF clients and servers"
SECTION = "libs"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://LICENSE;md5=08a5578c9bab06fb2ae84284630b973f"

SRC_URI = "https://github.com/CESNET/libnetconf2/archive/refs/tags/v${PV}.zip"
SRC_URI[md5sum] = "c034b5b96bacc6b0e3ebc9c04cbf318a"
SRC_URI[sha256sum] = "89a1ae32074da8ee6dae6e226813fcfdf4e67ce6124c81469970ebe7e009c11a"

S = "${WORKDIR}/${PN}-${PV}"

DEPENDS = "libssh openssl libyang libxcrypt"

RDEPENDS_${PN} += "openssl-bin "

inherit cmake pkgconfig

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
EXTRA_OECMAKE = " -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE:String=Release "

BBCLASSEXTEND = "native nativesdk"