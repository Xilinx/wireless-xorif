#
# This file is the libxobf recipe.
#

SUMMARY = "O-RAN Beamformer library"
SECTION = "PETALINUX/libs"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://Makefile \
	file://xobf_api.h \
	file://xobf_system.h \
	file://xobf_common.c \
	file://xobf_common.h \
	file://xobf_bf_func.c \
	file://xobf_bf_func.h \
	file://xobf_registers.c \
	file://xobf_registers.h \
	file://xobf_scheduler.c \
	file://xobf_beamweights.c \
	file://xobf_utils.c \
	file://xobf_utils.h \
	file://bf_regs.h \
	"

S = "${WORKDIR}"

DEPENDS = "libmetal"

do_compile() {
	oe_runmake
}

do_install() {
	install -d ${D}${libdir}
	oe_libinstall -so libxobf ${D}${libdir}

	install -d ${D}${includedir}/xobf
	install -m 0644 ${S}/xobf_api.h ${D}${includedir}/xobf/
	install -m 0644 ${S}/xobf_api.txt ${D}${includedir}/xobf/
}