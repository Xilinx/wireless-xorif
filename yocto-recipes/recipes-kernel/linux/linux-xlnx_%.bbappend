# This line adds additional kernel configurations
SRC_URI += "file://kernel.cfg"

# This line adds a kernel patch
#SRC_URI_append = " file://0001-Signed-off-by-Xu-Dong-xud-xilinx.com.patch"
SRC_URI_append = " file://0001-Re-apply-fix-to-changed-baseline-code.patch"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"
