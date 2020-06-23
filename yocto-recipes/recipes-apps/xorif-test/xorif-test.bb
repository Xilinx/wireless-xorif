#
# This file is the xorif-test recipe.
#
 
SUMMARY = "Simple xorif test application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
 
SRC_URI = " \
	file://Makefile \
	file://xorif_test.c \
	file://xorif_test.h \
	file://xorif_test_reg.c \
	file://xorif_test_lib.c \
	file://xorif_test_utils.c \
	file://oran_radio_if_v1_0_ctrl.h \
	"
 
S = "${WORKDIR}"
 
DEPENDS = "libxorif libmetal"
 
do_compile() {
	oe_runmake
}
 
do_install() {
	install -d ${D}${bindir}
	install -m 0755 xorif-test ${D}${bindir}
}