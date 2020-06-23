#
# This file is the xorif-startup recipe.
#

SUMMARY = "Simple xorif-startup application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://xorif-auto-start \
	file://xorif-startup.sh \
	file://runXorif.bash \
	"

S = "${WORKDIR}"

RDEPENDS_${PN} += "bash"
FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

inherit update-rc.d

INITSCRIPT_NAME = "xorif-auto-start"
INITSCRIPT_PARAMS = "start 99 S ."

do_install() {
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${S}/xorif-auto-start ${D}${sysconfdir}/init.d/xorif-auto-start
	install -d ${D}/${bindir}
	install -m 0755 ${S}/xorif-startup.sh ${D}/${bindir}
	install -m 0755 ${S}/runXorif.bash ${D}/${bindir}
}

FILES_${PN} += "${sysconfdir}/*"