#
# This file is the xorif-test recipe.
#

SUMMARY = "XORIF test scripts & tools"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://test_lib_api.py \
	file://test_lib_config.py \
	file://test_reg_fhi.py \
	file://test_reg_bf.py \
	file://test_pcaps.py \
	file://utils.py \
	file://profile \
	file://pg370_example1.txt \
	file://pg370_example2.txt \
	file://simple.txt \
	file://Makefile \
	file://oran-codec-bin-load.c \
	file://torwave.py \
	file://radiosink.py \
	file://xorifapp.py \
	"

S = "${WORKDIR}"

do_compile() {
	oe_runmake
}

do_install() {
    install -d ${D}/home/root
    install -m 0755 ${S}/*.py ${D}/home/root/
    install -m 0755 ${S}/profile ${D}/home/root/.profile
	install -m 0755 ${S}/*.txt ${D}/home/root/
	install -d ${D}${bindir}
	install -m 0755 memload ${D}${bindir}
}

FILES_${PN}  = "/home/root/*"
FILES_${PN} += "/home/root/.profile"
FILES_${PN} += "/usr/bin/memload"
