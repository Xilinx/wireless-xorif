#
# This file is the xorif-app recipe.
#

SUMMARY = "O-RAN Radio Interface and Beamformer Test application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://Makefile \
	file://xorif_app.c \
	file://xorif_app.h \
	file://xorif_file.c \
	file://xorif_command.c \
	file://xorif_socket.c \
	file://ecpri.c \
	file://ecpri_proto.c \
	file://ecpri_proto.h \
	file://ecpri_str.h \
	"

S = "${WORKDIR}"

DEPENDS = "libxorif libmetal"

do_compile() {
	oe_runmake
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 xorif-app ${D}${bindir}
}