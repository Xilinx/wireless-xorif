#
# This file is the libxocp recipe.
#

SUMMARY = "O-RAN Channel Processor library"
SECTION = "PETALINUX/libs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://Makefile \
	file://xocp_api.h \
	file://xocp.c \
	file://xocp.h \
	file://xocp_registers.c \
	file://xocp_registers.h \
	file://xocp_utils.c \
	file://xocp_utils.h \
	file://xorif_chan_proc_ctrl.h \
	file://pylibxocp.py \
	"

S = "${WORKDIR}"

DEPENDS = "libmetal"

do_compile() {
	oe_runmake
}

do_install() {
	install -d ${D}${libdir}
	oe_libinstall -so libxocp ${D}${libdir}

	install -d ${D}${includedir}/xorif
	install -m 0644 ${S}/xocp_api.h ${D}${includedir}/xorif/
	install -m 0644 ${S}/xocp_api.txt ${D}${includedir}/xorif/
	install -d ${D}/usr/share/xorif/
	install -m 0755 ${S}/pylibxocp.py ${D}/usr/share/xorif/
}

FILES:${PN} += " /usr/share/xorif/* "