#
# This file is the xmplane-app recipe.
#

SUMMARY = "Xilinx M-Plane Application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://fault_management.py \
	file://file_management.py \
	file://notification.py \
	file://setup.sh \
	file://supervision.py \
	file://utils.py \
	file://xmplane-app.py \
    file://modules/ \
	"

S = "${WORKDIR}"

RDEPENDS_${PN} += " bash python3-asyncssh python3-libyang-python "

do_install() {
	install -d ${D}/home/root/xmplane
	install -m 0755 ${S}/*.py ${D}/home/root/xmplane
	install -m 0755 ${S}/*.sh ${D}/home/root/xmplane
	install -d ${D}/usr/share/yang/modules/o-ran
	install -m 0755 ${S}/modules/*.yang ${D}/usr/share/yang/modules/o-ran/
}

FILES_${PN}  = " /home/root/xmplane/* "
FILES_${PN} += " /usr/share/yang/modules/o-ran/* "