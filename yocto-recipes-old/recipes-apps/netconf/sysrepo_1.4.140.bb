SUMMARY = "YANG-based configuration and operational state data store for Unix/Linux applications."
SECTION = "libs"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=e3fc50a88d0a364313df4b21ef20c29e"

SRC_URI = "https://github.com/sysrepo/sysrepo/archive/refs/tags/v${PV}.zip"
SRC_URI[md5sum] = "4c037479f8b1cee1e57d26be72e27b50"
SRC_URI[sha256sum] = "6fc31eaf09114f9d4c28191278faeffac463dd0e46aa944872a3dac57f1683fa"

S = "${WORKDIR}/${PN}-${PV}"

DEPENDS = "libyang libnetconf2"

FILES_${PN} += "/usr/share/yang/* /run/sysrepo*"

inherit cmake pkgconfig

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
EXTRA_OECMAKE = " -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE:String=Release -DREPO_PATH:PATH=/etc/sysrepo "

BBCLASSEXTEND = "native nativesdk" 

do_install_append () {
    install -d ${D}/etc/sysrepo/data/notifications
    install -d ${D}/etc/sysrepo/yang
    install -o root -g root ${S}/modules/ietf-netconf-notifications.yang ${D}/etc/sysrepo/yang/ietf-netconf-notifications@2012-02-06.yang
    install -o root -g root ${S}/modules/ietf-netconf-with-defaults.yang ${D}/etc/sysrepo/yang/ietf-netconf-with-defaults@2011-06-01.yang
    install -o root -g root ${S}/modules/ietf-netconf.yang ${D}/etc/sysrepo/yang/ietf-netconf@2011-06-01.yang
    install -d ${D}/usr/lib/sysrepo/plugins
}