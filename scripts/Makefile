### MAKEFILE FOR PETALINUX BUILD SYSTEM ###

# Set up environment
SRC_DIR = ../src
RCP_DIR = ../yocto-recipes
OUT_DIR = ../output
XSA_DIR = ../xsa
TOOL_VER = $(shell printenv PETALINUX_VER | perl -lne 'if (m/([0-9]+)\.([0-9]+)/) {print "$$1_$$2"}')
PLNX_VER = $(shell printenv PETALINUX_VER)
#DTC = $(PETALINUX)/components/yocto/source/arm/buildtools/sysroots/x86_64-petalinux-linux/usr/bin/dtc
DTC = $(PETALINUX)/components/yocto/buildtools/sysroots/x86_64-petalinux-linux/usr/bin/dtc

### CONFIGURATION SECTION ###

# Add kernel modules (e.g. drivers) here
override MODULES += xlnx-ptp-timer

# Add apps / libs here
override APPS += libxorif
override APPS += xorif-app
#override APPS += xorif-test

# Add scripts here
override INSTALL += xorif-startup
override INSTALL += xorif-scripts

# Add rootfs (=Y) config options here
override ROOTFS_Y += peekpoke
override ROOTFS_Y += perl
override ROOTFS_Y += perl-module-findbin
override ROOTFS_Y += python
override ROOTFS_Y += python-dev
override ROOTFS_Y += python-ctypes
override ROOTFS_Y += python-unittest
override ROOTFS_Y += python-argparse
override ROOTFS_Y += python3
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
override ROOTFS_Y += xorif-test-dev
override ROOTFS_Y += xorif-test-dbg
endif

# Add rootfs (=N) config options here
override ROOTFS_N +=

# Extra bits
override EXTRAS += recipes-kernel/linux
override EXTRAS += recipes-core/init-ifupdown

# Add help / usage guide here
usage:
	@echo "Usage: make [target]"
	@echo "Example: make zcu111_om5_exs"
	@echo
	@echo "Supported targets:"
	@echo "    all"
	@echo "    clean_all"
	@echo "    zcu102_om5_exd"
	@echo "    zcu102_om5_exs"
	@echo "    zcu111_om5_exd"
	@echo "    zcu111_om5_exs"
	@echo "    vck190_test"
	@echo
	@echo "Notes:"
	@echo "    Board: [zcu102 | zcu111 | vck190]"
	@echo "    Mode: [om5 = ORAN]"
	@echo "    Design: [exd = example design | exs = example system]"

### MAIN RULES AND RECIPES ###

# Recipe to add kernel module
$(MODULES):
	@echo Creating kernel module $@
	petalinux-create --project $(PLX_DIR) --type modules --name $@ --enable --force
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-modules/$@/files/$@.c
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-modules/$@/$@.bb
	cp $(SRC_DIR)/$@/* $(PLX_DIR)/project-spec/meta-user/recipes-modules/$@/files/.
	cp $(RCP_DIR)/recipes-modules/$@/$@.bb $(PLX_DIR)/project-spec/meta-user/recipes-modules/$@/.

# Recipe to add app / lib
$(APPS):
	@echo Creating application $@
	petalinux-create --project $(PLX_DIR) --type apps --template c --name $@ --enable --force
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/$@.c
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/$@.bb
	cp $(SRC_DIR)/$@/* $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/.
	cp $(RCP_DIR)/recipes-apps/$@/$@.bb $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/.
	echo CONFIG_$@-dbg >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig
	echo CONFIG_$@-dev >> $(PLX_DIR)/project-spec/meta-user/conf/user-rootfsconfig

# Recipe to add pre-built (e.g. binary, scripts, etc.)
$(INSTALL):
	@echo Creating pre-built $@
	petalinux-create --project $(PLX_DIR) --type apps --template install --name $@ --enable --force
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/*
	-rm $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/$@.bb
	cp $(SRC_DIR)/$@/* $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/files/.
	cp $(RCP_DIR)/recipes-apps/$@/$@.bb $(PLX_DIR)/project-spec/meta-user/recipes-apps/$@/.

# Recipe for extra bits that are just copied wholesale into meta-user directory
$(EXTRAS):
	@echo Adding extra files for $@
	mkdir -p $(PLX_DIR)/project-spec/meta-user/$@
	cp -pr $(RCP_DIR)/$@/* $(PLX_DIR)/project-spec/meta-user/$@/.

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
	@echo Creating Petalinux project
	mkdir -p $(shell dirname $@)
ifdef BSP
	cd $(shell dirname $@) && petalinux-create --type project --source $(BSP) --name $(shell basename $@)
	petalinux-config --project $(PLX_DIR) --silentconfig
else ifeq ($(BOARD),zcu102)
	cd $(shell dirname $@) && petalinux-create --type project --template zynqMP --name $(shell basename $@)
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME="zcu102-rev1.0"/' $@/project-spec/configs/config
	perl -p -i -e 's/^.*YOCTO_MACHINE_NAME.*/YOCTO_MACHINE_NAME="zcu102-zynqmp"/' $@/project-spec/configs/config
ifeq ($(PLNX_VER),2019.2)
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_zcu102_rev1_0_defconfig"/' $@/project-spec/configs/config
else
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_virt_defconfig"/' $@/project-spec/configs/config
endif
else ifeq ($(BOARD),zcu111)
	cd $(shell dirname $@) && petalinux-create --type project --template zynqMP --name $(shell basename $@)
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME="zcu111-reva"/' $@/project-spec/configs/config
	perl -p -i -e 's/^.*YOCTO_MACHINE_NAME.*/YOCTO_MACHINE_NAME="zcu111-zynqmp"/' $@/project-spec/configs/config
ifeq ($(PLNX_VER),2019.2)
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_zcu111_revA_defconfig"/' $@/project-spec/configs/config
else
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_virt_defconfig"/' $@/project-spec/configs/config
endif
else ifeq ($(BOARD),vck190)
	cd $(shell dirname $@) && petalinux-create --type project --template versal --name $(shell basename $@)
	perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_MACHINE_NAME.*/CONFIG_SUBSYSTEM_MACHINE_NAME=""/' $@/project-spec/configs/config
	perl -p -i -e 's/^.*YOCTO_MACHINE_NAME.*/YOCTO_MACHINE_NAME="vck190-versal"/' $@/project-spec/configs/config
	#perl -p -i -e 's/^.*CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET.*/CONFIG_SUBSYSTEM_UBOOT_CONFIG_TARGET="xilinx_zynqmp_virt_defconfig"/' $@/project-spec/configs/config
endif

# Update with the hardware description (e.g. HDF or XSA) if it exists
ifneq ("$(wildcard $(XSA_DIR)/$(TARGET)/system.xsa)","")
$(PLX_DIR)/project-spec/hw-description/system.xsa: $(XSA_DIR)/$(TARGET)/system.xsa
	@echo Configuring with HDF / XSA file
	petalinux-config --project $(PLX_DIR) --get-hw-description $(XSA_DIR)/$(TARGET) --silentconfig
endif

# Generate DTS from DTB
$(PLX_DIR)/images/linux/system.dts: $(PLX_DIR)/images/linux/system.dtb
	$(DTC) -I dtb -O dts -o $(PLX_DIR)/images/linux/system.dts $(PLX_DIR)/images/linux/system.dtb

# Add missing file required for QEMU
#$(PLX_DIR)/pre-built/linux/images/pmu_rom_qemu_sha3.elf: extras/pmu_rom_qemu_sha3.elf
#	mkdir -p $(PLX_DIR)/pre-built/linux/images
#	cp extras/pmu_rom_qemu_sha3.elf $(PLX_DIR)/pre-built/linux/images/.

.PHONY: create config1 config2 build package clean mrproper

# Apply DTS additions
# This consists of 1) universal additions, 2) board-specific additions, 3) design-specific additions
create: $(PLX_DIR) $(PLX_DIR)/project-spec/hw-description/system.xsa
	cp $(RCP_DIR)/recipes-bsp/device-tree/system-user.dtsi $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
ifeq ($(BOARD),zcu102)
	cat $(RCP_DIR)/recipes-bsp/device-tree/system-user_zcu102.dtsi >> $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
else ifeq ($(BOARD),zcu111)
	cat $(RCP_DIR)/recipes-bsp/device-tree/system-user_zcu111.dtsi >> $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
else ifeq ($(BOARD),vck190)
	cat $(RCP_DIR)/recipes-bsp/device-tree/system-user_vck190.dtsi >> $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
endif
ifeq ($(XXV),1)
	cat $(RCP_DIR)/recipes-bsp/device-tree/system-user_xxv.dtsi >> $(PLX_DIR)/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi
endif

# Configure the rootfs
config1: create $(APPS) $(MODULES) $(INSTALL)
	@echo Configuring 1 $(PLX_DIR)
	petalinux-config --project $(PLX_DIR) --component rootfs --silentconfig

# Configure (and patch) the kernel and any other steps
config2: config1 $(ROOTFS_Y) $(ROOTFS_N) $(EXTRAS)
	@echo Configuring 2 $(PLX_DIR)
	petalinux-config --project $(PLX_DIR) --silentconfig

# Build
build: config2
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
	@echo Deep-cleaning $(PLX_DIR)
	petalinux-build --project $(PLX_DIR) -x clean
endif

# Deep-clean build area, state cache and TMPDIR
mrproper:
ifneq ("$(wildcard $(PLX_DIR))","")
	@echo Deep-cleaning $(PLX_DIR)
	petalinux-build --project $(PLX_DIR) -x mrproper
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

zcu111_om5_exd:
	$(MAKE) BOARD=zcu111 TARGET=$@ DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

zcu111_om5_exs:
	$(MAKE) BOARD=zcu111 TARGET=$@ XXV=1 DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

vck190_test:
	$(MAKE) BOARD=vck190 TARGET=$@ DBG=1 \
	PLX_DIR=$(OUT_DIR)/$@_$(TOOL_VER)/petalinux \
	package

all: zcu102_om5_exd zcu102_om5_exs zcu111_om5_exd zcu111_om5_exs vck190_test

clean_all:
	@echo Cleaning all targets
	$(MAKE) PLX_DIR="$(OUT_DIR)/zcu102_om5_exd_$(TOOL_VER)/petalinux" mrproper
	$(MAKE) PLX_DIR="$(OUT_DIR)/zcu102_om5_exs_$(TOOL_VER)/petalinux" mrproper
	$(MAKE) PLX_DIR="$(OUT_DIR)/zcu111_om5_exd_$(TOOL_VER)/petalinux" mrproper
	$(MAKE) PLX_DIR="$(OUT_DIR)/zcu111_om5_exs_$(TOOL_VER)/petalinux" mrproper
	$(MAKE) PLX_DIR="$(OUT_DIR)/vck190_test_$(TOOL_VER)/petalinux" mrproper

# TODO
# - VCK190 build needs additional config, but this information is not available yet!
# - Only add the rootfs_config and user-rootfsconfig lines if they don't already exist
# - Change the clean target to loop through directories? Or maybe use same inputs as all target?
# - Make kernel configs / patches work similar as rootfs?
