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
	file://utils.py \
	file://profile \
	"

S = "${WORKDIR}"

do_install() {
    install -d ${D}/home/root
    install -m 0755 ${S}/*.py     ${D}/home/root/
    install -m 0755 ${S}/profile  ${D}/home/root/.profile
}

FILES_${PN}  = "/home/root/*"
FILES_${PN} += "/home/root/.profile"
