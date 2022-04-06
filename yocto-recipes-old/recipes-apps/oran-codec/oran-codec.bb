#
# This file is the oran-codec recipe.
#

SUMMARY = "Simple oran-codec application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRCREV = "${AUTOREV}"
SRC_URI = "git://gitenterprise.xilinx.com/wireless-apps/oran-codec.git;protocol=http"
SRC_URI[md5sum] = "e2dcb1ac290962e3c9626276bfad9419"
SRC_URI[sha256sum] = "cecb2e5c25fe8d512d58a23ce2ccda421cf166587316857264de42b2c7f4fea7"

S = "${WORKDIR}/git"

RDEPENDS_${PN} += " perl"

do_compile() {
	oe_runmake -C ${S}/src/src; 
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 ${S}/src/src/oran-codec ${D}${bindir}
	install -m 0755 ${S}/src/oran-codec.pl ${D}${bindir}
}