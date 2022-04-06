#
# This file is the xorif-scripts recipe.
#

SUMMARY = "Simple xorif-scripts application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://fix_eth0_eth1.sh \
	file://xroe-config-XXV-ptp.sh \
	file://xroe-ptp4lsyncE.cfg \
    "

S = "${WORKDIR}"

do_install() {
	install -d ${D}/${bindir}
	install -m 0755 ${S}/fix_eth0_eth1.sh ${D}/${bindir}
	install -m 0755 ${S}/xroe-config-XXV-ptp.sh ${D}/${bindir}
	install -m 0755 ${S}/xroe-ptp4lsyncE.cfg ${D}/${bindir}
}