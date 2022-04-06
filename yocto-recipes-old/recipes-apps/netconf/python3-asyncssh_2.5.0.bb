inherit pypi setuptools3

SUMMARY = "Asynchronous SSHv2 client and server library."
AUTHOR = "Ron Frederick"
HOMEPAGE = "https://github.com/ronf/asyncssh/"
LICENSE = "EPL-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=d9fc0efef5228704e7f5b37f27192723"

RDEPENDS_${PN} += "\
    ${PYTHON_PN}-cryptography \
"

SRC_URI[md5sum] = "ec6fb9da9ff9fd7f95de9c83217379fe"
SRC_URI[sha256sum] = "0b65e2af73a2e39a271bd627abbe4f7e4b0345486ed403e65987d79c72fcb70b"