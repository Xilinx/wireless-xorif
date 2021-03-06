### MAKEFILE FOR PETALINUX BUILD SYSTEM ###

# Set up environment
SRC_DIR = ../src
RCP_DIR = ../yocto-recipes
OUT_DIR = ../output
XSA_DIR = ../xsa
TOOL_VER = $(shell printenv PETALINUX_VER | perl -lne 'if (m/([0-9]+)\.([0-9]+)/) {print "$$1_$$2"}')
PLNX_VER = $(shell printenv PETALINUX_VER)
DTC = $(PETALINUX)/components/yocto/buildtools/sysroots/x86_64-petalinux-linux/usr/bin/dtc

### CONFIGURATION SECTION ###

# Define targets for "all" here
#ALL = zcu102_om5_exs zcu102_om5_exd zcu111_om5_exs zcu111_om5_exd zcu111_om5_10_slv_exd vck190_hw
ALL = zcu102_om5_exs zcu111_om5_exs

# Add kernel modules (e.g. drivers) here
override MODULES += xlnx-ptp-timer

# Add apps / libs here
override APPS += libxorif
override APPS += xorif-app

# Add scripts here
ifeq ($(XXV),1)
override INSTALL += xorif-startup
override INSTALL += xorif-scripts
endif
#override APPS += xorif-test

# Add rootfs (=Y) config options here
override ROOTFS_Y += linuxptp
override ROOTFS_Y += peekpoke
override ROOTFS_Y += perl
override ROOTFS_Y += python
override ROOTFS_Y += python-dev
override ROOTFS_Y += python-argparse
override ROOTFS_Y += python3
override ROOTFS_Y += python3-dev
override ROOTFS_Y += netcat
override ROOTFS_Y += tcpdump
override ROOTFS_Y += libmetal
ifeq ($(DBG),1)
override ROOTFS_Y += git
override ROOTFS_Y += packagegroup-core-buildessential
override ROOTFS_Y += libgcc
override ROOTFS_Y += libgcc-dev
override ROOTFS_Y += libgcc-dbg
override ROOTFS_Y += libstdcPLUSPLUS
override ROOTFS_Y += libstdcPLUSPLUS-dev
override ROOTFS_Y += gdb
override ROOTFS_Y += gdbserver
override ROOTFS_Y += libmetal-dev
override ROOTFS_Y += libmetal-dbg
override ROOTFS_Y += libxorif-dev
override ROOTFS_Y += libxorif-dbg
override ROOTFS_Y += xorif-app-dev
override ROOTFS_Y += xorif-app-dbg
endif

# Add rootfs (=N) config options here
override ROOTFS_N +=

# Extra bits
override EXTRAS += linux
override EXTRAS += init-ifupdown
override EXTRAS += perl-modules
override EXTRAS += perl-module-findbin
override EXTRAS += perl-module-carp
override EXTRAS += perl-module-lib
#override EXTRAS += python3-modules
#override EXTRAS += oran-codec

# Add help / usage guide here
usage:
	@echo "Usage: make [target]"
	@echo "Example: make zcu111_om5_exs"
	@echo
	@echo "Supported targets:"
	@echo "    all"
	@echo "    clean_all"
	@for x in $(ALL); do \
		echo "    $${x}";\
	done
	@echo
	@echo "Notes:"
	@echo "    Board: [zcu102 | zcu111]"
	@echo "    Mode: [om5 = ORAN]"
	@echo "    Design: [exs = example system]"

### MAIN RULES AND RECIPES ###

oran-codec:
	@echo Creating application $@
	petalinux-create --project $(PLX_DIR) --type apps --template c --name $@ --enable --force
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/*
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/$@.bb
	cp -p $(RCP_DIR)/recipes-apps/$@/*.bb $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/.
	echo CONFIG_$@-dbg >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	echo CONFIG_$@-dev >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig

perl-modules:
	@echo Adding changes for $@
	echo CONFIG_$@ >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	petalinux-config --project $(PLX_DIR) --silentconfig
	perl -i -pe 's/^# CONFIG_$@\s.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_$@=n.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config

perl-module-findbin:
	@echo Adding changes for $@
	echo CONFIG_$@ >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	petalinux-config --project $(PLX_DIR) --silentconfig
	perl -i -pe 's/^# CONFIG_$@\s.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_$@=n.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config

perl-module-carp:
	@echo Adding changes for $@
	echo CONFIG_$@ >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	petalinux-config --project $(PLX_DIR) --silentconfig
	perl -i -pe 's/^# CONFIG_$@\s.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_$@=n.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config

perl-module-lib:
	@echo Adding changes for $@
	echo CONFIG_$@ >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	petalinux-config --project $(PLX_DIR) --silentconfig
	perl -i -pe 's/^# CONFIG_$@\s.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_$@=n.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config

python3-modules:
	@echo Adding changes for $@
	mkdir -p $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@
	cp -p $(RCP_DIR)/recipes-apps/$@/*.bb $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/.
	echo CONFIG_python3-pyro4 >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	echo CONFIG_python3-cffi >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	echo CONFIG_python3-pytest >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	petalinux-config --project $(PLX_DIR) --silentconfig
	perl -i -pe 's/^# CONFIG_python3-pyro4\s.*\n/CONFIG_python3-pyro4=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_python3-pyro4=n.*\n/CONFIG_python3-pyro4=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^# CONFIG_python3-cffi\s.*\n/CONFIG_python3-cffi=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_python3-cffi=n.*\n/CONFIG_python3-cffi=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^# CONFIG_python3-pytest\s.*\n/CONFIG_python3-pytest=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_python3-pytest=n.*\n/CONFIG_python3-pytest=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config

linux:
	@echo Adding changes for $@
	mkdir -p $(PLX_DIR)/project-spec/meta-user/recipes-kernel/$@
	cp -pr $(RCP_DIR)/recipes-kernel/$@/* $(PLX_DIR)/project-spec/meta-user/recipes-kernel/$@/.

init-ifupdown:
	@echo Adding changes for $@
	mkdir -p $(PLX_DIR)/project-spec/meta-user/recipes-core/$@
	cp -pr $(RCP_DIR)/recipes-core/$@/* $(PLX_DIR)/project-spec/meta-user/recipes-core/$@/.

# Recipe to add kernel module
$(MODULES):
	@echo Creating kernel module $@
	petalinux-create --project $(PLX_DIR) --type modules --name $@ --enable --force
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-modules/$@/files/$@.c
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-modules/$@/$@.bb
	cp -p $(SRC_DIR)/$@/* $(PLX_DIR)/project-spec/meta-user/recipes-modules/$@/files/.
	cp -p $(RCP_DIR)/recipes-modules/$@/*.bb $(PLX_DIR)/project-spec/meta-user/recipes-modules/$@/.

# Recipe to add app / lib
$(APPS):
	@echo Creating application $@
	petalinux-create --project $(PLX_DIR) --type apps --template c --name $@ --enable --force
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/*
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/$@.bb
	cp -p $(SRC_DIR)/$@/* $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/.
	cp -p $(RCP_DIR)/recipes-apps/$@/*.bb $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/.
	echo CONFIG_$@-dbg >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	echo CONFIG_$@-dev >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig

# Recipe to add pre-built (e.g. binary, scripts, etc.)
$(INSTALL):
	@echo Creating pre-built $@
	petalinux-create --project $(PLX_DIR) --type apps --template install --name $@ --enable --force
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/*
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/$@.bb
	cp -p $(SRC_DIR)/$@/* $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/.
	cp -p $(RCP_DIR)/recipes-apps/$@/*.bb $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/.

# Recipe for rootfs (=Y) config option
$(ROOTFS_Y):
	@echo Setting rootfs $@ to Y
	perl -i -pe 's/^# CONFIG_$@\s.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_$@=n.*\n/CONFIG_$@=y\n/' $(PLX_DIR)/project-spec/configs/rootfs_config

# Recipe for rootfs (=N) config option
$(ROOTFS_N):
	@echo Setting rootfs $@ to N
	perl -i -pe 's/^# CONFIG_$@\s.*\n/CONFIG_$@=n\n/' $(PLX_DIR)/project-spec/configs/rootfs_config
	perl -i -pe 's/^CONFIG_$@=y.*\n/CONFIG_$@=n\n/' $(PLX_DIR)/project-spec/configs/rootfs_config

# Create the PetaLinux project, either from a template or a BSP (if supplied)
$(PLX_DIR):
	@echo Creating Petalinux project $(PLX_DIR)
	mkdir -p $(shell dirname $@)
ifdef BSP
	cd $(shell dirname $@) && petalinux-create --type project --source $(BSP) --name $(shell basename $@) --force
	petalinux-config --project $(PLX_DIR) --silentconfig
else ifeq ($(BOARD),zcu102)
	cd $(shell dirname $@) && petalinux-create --type project --template zynqMP --name $(shell basename $@) --force
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME="zcu102-rev1.0"/' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_YOCTO_MACHINE_NAME.*/CONFIG_YOCTO_MACHINE_NAME="zcu102-zynqmp"/' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_virt_defconfig"/' $@/project-spec/configs/config
else ifeq ($(BOARD),zcu111)
	cd $(shell dirname $@) && petalinux-create --type project --template zynqMP --name $(shell basename $@) --force
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME="zcu111-reva"/' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_YOCTO_MACHINE_NAME.*/CONFIG_YOCTO_MACHINE_NAME="zcu111-zynqmp"/' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_virt_defconfig"/' $@/project-spec/configs/config
else ifeq ($(BOARD),vck190)
	cd $(shell dirname $@) && petalinux-create --type project --template versal --name $(shell basename $@) --force
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME=""/' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_YOCTO_MACHINE_NAME.*/CONFIG_YOCTO_MACHINE_NAME="vck190-versal"/' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_versal_virt_defconfig"/' $(PLX_DIR)/project-spec/configs/config
endif

# Update with the hardware description (e.g. HDF or XSA) if it exists
ifneq ("$(wildcard $(XSA_DIR)/$(TARGET)/system.xsa)","")
$(PLX_DIR)/project-spec/hw-description/system.xsa: $(XSA_DIR)/$(TARGET)/system.xsa
	@echo Setting hw-description $(PLX_DIR)
	petalinux-config --project $(PLX_DIR) --get-hw-description $(XSA_DIR)/$(TARGET) --silentconfig
endif

# Generate DTS from DTB
$(PLX_DIR)/images/linux/system.dts: $(PLX_DIR)/images/linux/system.dtb
	$(DTC) -I dtb -O dts -o $(PLX_DIR)/images/linux/system.dts $(PLX_DIR)/images/linux/system.dtb

.PHONY: config1 config2 config3 build package clean mrproper

# Configure step 1 (hw description, base rootfs, apps, etc.)
config1: $(PLX_DIR) $(PLX_DIR)/project-spec/hw-description/system.xsa $(APPS) $(MODULES) $(INSTALL)
	@echo Configuring 1 $(PLX_DIR)
	petalinux-config --project $(PLX_DIR) --component rootfs --silentconfig

# Configure step 2 (rootfs options, etc.)
config2: config1 $(ROOTFS_Y) $(ROOTFS_N) $(EXTRAS)
	@echo Configuring 2 $(PLX_DIR)
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_HOSTNAME.*/CONFIG_SUBSYSTEM_HOSTNAME="$(TARGET)"/' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_PRODUCT.*/CONFIG_SUBSYSTEM_PRODUCT="$(TARGET)"/' $(PLX_DIR)/project-spec/configs/config
ifeq ($(TARGET),vck190_hw_emu)
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_ROOTFS_INITRD=.*/# CONFIG_SUBSYSTEM_ROOTFS_INITRD is not set/' $(PLX_DIR)/project-spec/configs/config
	sed -i '/CONFIG_SUBSYSTEM_INITRD_RAMDISK_LOADADDR/d' $(PLX_DIR)/project-spec/configs/config
	sed -i '/CONFIG_SUBSYSTEM_RAMDISK_IMAGE/d' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_ROOTFS_EXT4.*/CONFIG_SUBSYSTEM_ROOTFS_EXT4=y/' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_RFS_FORMATS.*/CONFIG_SUBSYSTEM_RFS_FORMATS="tar.gz cpio.gz cpio.gz.u-boot ext4 ext4.gz jffs2"/' $(PLX_DIR)/project-spec/configs/config
	sed -i '/CONFIG_SUBSYSTEM_ROOTFS_OTHER/a CONFIG_SUBSYSTEM_SDROOT_DEV="/dev/mmcblk0p2"' $(PLX_DIR)/project-spec/configs/config
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_BOOTARGS_AUTO=.*/# CONFIG_SUBSYSTEM_BOOTARGS_AUTO is not set/' $(PLX_DIR)/project-spec/configs/config
	sed -i '/CONFIG_SUBSYSTEM_BOOTARGS_AUTO/a CONFIG_SUBSYSTEM_USER_CMDLINE="console=ttyAMA0 earlycon=pl011,mmio32,0xFF000000,115200n8 clk_ignore_unused mem=2G root=/dev/mmcblk0p2 rw rootwait cma=512M"' $(PLX_DIR)/project-spec/configs/config
endif
	petalinux-config --project $(PLX_DIR) --silentconfig

# Configure step 3 (device tree mods)
config3: config2
	@echo Configuring 3 $(PLX_DIR)
	cp $(RCP_DIR)/recipes-bsp/device-tree/system-user.dtsi $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
ifdef BSP
else ifeq ($(BOARD),zcu102)
	cat $(RCP_DIR)/recipes-bsp/device-tree/system-user_zcu102.dtsi >> $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
else ifeq ($(BOARD),zcu111)
	cat $(RCP_DIR)/recipes-bsp/device-tree/system-user_zcu111.dtsi >> $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
else ifeq ($(BOARD),vck190)
	cat $(RCP_DIR)/recipes-bsp/device-tree/system-user_vck190.dtsi >> $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
endif
ifeq ($(XXV),1)
	cat $(RCP_DIR)/recipes-bsp/device-tree/system-user_xxv.dtsi >> $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
endif

# Build
build: config3
	@echo Building $(PLX_DIR)
	petalinux-build --project $(PLX_DIR)

# Packaging and any other post-build steps
package: build $(PLX_DIR)/images/linux/system.dts
	@echo Packaging $(PLX_DIR)
ifeq ($(BOARD),vck190)
	petalinux-package --project $(PLX_DIR) --boot --u-boot --force
else
	petalinux-package --project $(PLX_DIR) --boot --u-boot --fpga --force
endif

# Clean build area
clean:
ifneq ("$(wildcard $(PLX_DIR))","")
	@echo Cleaning $(PLX_DIR)
	petalinux-build --project $(PLX_DIR) -x clean -f
endif

# Deep-clean build area, state cache and TMPDIR
mrproper:
ifneq ("$(wildcard $(PLX_DIR))","")
	@echo Deep-cleaning $(PLX_DIR)
	petalinux-build --project $(PLX_DIR) -x mrproper -f
endif

#### MAIN TARGETS BELOW ####

zcu102_bsp:
	$(MAKE) BOARD=zcu102 TARGET=$@ \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	BSP=$(PETALINUX)/../../bsp/release/xilinx-zcu102-v$(PLNX_VER)-final.bsp \
	package

zcu111_bsp:
	$(MAKE) BOARD=zcu111 TARGET=$@ \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	BSP=$(PETALINUX)/../../bsp/release/xilinx-zcu111-v$(PLNX_VER)-final.bsp \
	package

vck190_bsp:
	$(MAKE) BOARD=vck190 TARGET=$@ \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	BSP=$(PETALINUX)/../../bsp/release/xilinx-vck190-v$(PLNX_VER)-final.bsp \
	package

zcu102_om5_exd:
	$(MAKE) BOARD=zcu102 TARGET=$@ DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

zcu102_om5_exs:
	$(MAKE) BOARD=zcu102 TARGET=$@ XXV=1 DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

zcu102_om5_e4_exs:
	$(MAKE) BOARD=zcu102 TARGET=$@ XXV=1 DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

zcu111_om5_exd:
	$(MAKE) BOARD=zcu111 TARGET=$@ DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

zcu111_om5_10_slv_exd:
	$(MAKE) BOARD=zcu111 TARGET=$@ DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

zcu111_om5_exs:
	$(MAKE) BOARD=zcu111 TARGET=$@ XXV=1 DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

zcu111_om5_e4_exs:
	$(MAKE) BOARD=zcu111 TARGET=$@ XXV=1 DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

vck190_hw:
	$(MAKE) BOARD=vck190 TARGET=$@ DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

vck190_hw_emu:
	$(MAKE) BOARD=vck190 TARGET=$@ DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

all: $(ALL)

clean_all:
	@echo Cleaning targets
	for x in $(ALL); do \
		$(MAKE) PLX_DIR="$(OUT_DIR)/$${x}_$(TOOL_VER)/petalinux" mrproper; \
	done

# TODO
# - Only add the rootfs_config and user-rootfsconfig lines if they don't already exist
