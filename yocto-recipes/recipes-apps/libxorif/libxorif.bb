#
# This file is the libxorif recipe.
#

SUMMARY = "O-RAN Radio Interface library"
SECTION = "PETALINUX/libs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://Makefile \
	file://xorif_api.h \
	file://xorif_system.h \
	file://xorif_common.c \
	file://xorif_common.h \
	file://xorif_fh_func.c \
	file://xorif_fh_func.h \
	file://xorif_utils.c \
	file://xorif_utils.h \
	file://oran_radio_if_v2_0_ctrl.h \
	"

S = "${WORKDIR}"

DEPENDS = "libmetal"

do_compile() {
	oe_runmake
}

do_install() {
	install -d ${D}${libdir}
	oe_libinstall -so libxorif ${D}${libdir}

	install -d ${D}${includedir}/xorif
	install -m 0644 ${S}/xorif_api.h ${D}${includedir}/xorif/
	install -m 0644 ${S}/xorif_api.txt ${D}${includedir}/xorif/
}