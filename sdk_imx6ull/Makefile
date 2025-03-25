
target_gcc=$(CROSS_COMPILE)gcc
wrkdir=$(CURDIR)/work
uboot_wrkdir=$(wrkdir)/uboot
uboot_srcdir=$(CURDIR)/uboot-imx
uboot_config=$(CURDIR)/conf/uboot_sqx_defconfig
uboot_imxdownload=$(CURDIR)/toolchain/imxdownload
uboot_images=$(CURDIR)/images/uboot.bin
linux_image_tftp := $(wrkdir)/tftp/zImage
linux_zImage := $(wrkdir)/linux/arch/arm/boot/zImage

linux_srcdir=$(CURDIR)/linux-imx
linux_wrkdir=$(wrkdir)/linux
linux_modules_wrkdir=$(wrkdir)/linux_modules
linux_config=$(CURDIR)/conf/linux_sqx_defconfig
linux_dtb=$(CURDIR)/conf/imx_linux.dtb
linux_tftp_dtb=$(wrkdir)/tftp/imx_linux.dtb
linux_dts=$(CURDIR)/linux-imx/arch/arm/boot/dts/imx6ull-14x14-evk-emmc.dts
linux_preproc_dts=$(wrkdir)/imx_linux.dts.preprocessed
linux_dtc=$(linux_wrkdir)/scripts/dtc/dtc
buildroot_src=$(CURDIR)/buildroot-2022.02.8
nfs_wrkdir=$(CURDIR)/nfs
busybox_src=$(buildroot_src)/output/build/busybox-1.35.0

.PHONY: uboot uboot_menuconfig uboot_load
uboot: $(uboot_srcdir) $(uboot_wrkdir)/.config
	$(MAKE) -C $(uboot_srcdir) O=$(uboot_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) all -j16
	cp $(uboot_wrkdir)/u-boot.bin $(uboot_images)

uboot_menuconfig: $(uboot_wrkdir)/.config $(uboot_srcdir)
	$(MAKE) -C $(uboot_srcdir) O=$(uboot_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) menuconfig
	$(MAKE) -C $(uboot_srcdir) O=$(uboot_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) savedefconfig
	cp $(dir $<)/defconfig $(uboot_config)

uboot_load:$(uboot_images)
	$(uboot_imxdownload) $(uboot_images) /dev/sdd

$(uboot_wrkdir)/.config: $(uboot_srcdir)
	mkdir -p $(uboot_wrkdir)
	cp $(uboot_config) $@
	$(MAKE) -C $(uboot_srcdir) O=$(uboot_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) olddefconfig

.PHONY: linux linux_menuconfig dtb linux_modules
linux:$(linux_wrkdir)/.config $(linux_srcdir)
	$(MAKE) -C $(linux_srcdir) O=$(linux_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) -j16
	cp $(linux_zImage) $(linux_image_tftp)

linux_menuconfig: $(linux_wrkdir)/.config $(linux_srcdir)
	$(MAKE) -C $(linux_srcdir) O=$(linux_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) menuconfig
	$(MAKE) -C $(linux_srcdir) O=$(linux_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) savedefconfig
	cp $(dir $<)/defconfig $(linux_config)

dtb: $(linux_dtb) $(linux_wrkdir)/.config
#dtc -I dts -O dtb -o $(CURDIR)/conf/imx_linux.dts $(linux_dtb)

linux_modules:$(linux_wrkdir)/.config $(linux_srcdir)
	$(MAKE) -C $(linux_srcdir) O=$(linux_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) modules -j16

$(linux_wrkdir)/.config: $(linux_srcdir)
	mkdir -p $(linux_wrkdir)
	cp $(linux_config) $@
	$(MAKE) -C $(linux_srcdir) O=$(linux_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) olddefconfig


$(linux_dtb): $(linux_dts) $(linux_wrkdir)/.config $(linux_tftp_dtb)
	$(MAKE) -C $(linux_srcdir) O=$(linux_wrkdir) CROSS_COMPILE=$(CROSS_COMPILE) dtbs
	cp $(linux_wrkdir)/arch/arm/boot/dts/imx6ull-14x14-evk-emmc.dtb $(linux_dtb)
	cp $(linux_wrkdir)/arch/arm/boot/dts/imx6ull-14x14-evk-emmc.dtb $(linux_tftp_dtb)


.PHONY: buildroot_menuconfig buildroot busybox_menuconfig
buildroot_menuconfig:
	cd $(buildroot_src) && $(MAKE) menuconfig

buildroot:$(buildroot_src)/.config
	cd $(buildroot_src) && sudo $(MAKE)
	cp -r $(buildroot_src)/output/images/rootfs.tar $(nfs_wrkdir)
	cd $(nfs_wrkdir) && tar -xvf rootfs.tar -C rootfs

busybox_menuconfig:$(busybox_src)
	cd $(buildroot_src) && sudo $(MAKE) busybox-menuconfig

busybox:$(busybox_src)
	cd $(buildroot_src) && sudo $(MAKE) show-targets

# $(linux_dtb): $(linux_dts)
# 	$(target_gcc) -E -nostdinc -undef -x assembler-with-cpp \
# 	-I $(linux_srcdir)/include $(linux_dts) -o $(linux_preproc_dts)
# 	dtc -I dts -O dtb $(linux_preproc_dts) -o $(linux_dtb)

.PHONY: cleanuboot cleanlinux cleandtb
cleanuboot:
	rm -rf $(uboot_wrkdir)

cleanlinux:
	rm -rf $(linux_wrkdir)

cleandtb:
	rm -rf $(linux_preproc_dts) $(linux_dtb)

help:
	@echo  '  uboot  		      - Compile uboot and put it in images file'
	@echo  '  uboot_menuconfig	  - Update current config utilising a menu based program'
	@echo  '  cleanuboot     	  - clean uboot in work file'
	@echo  '  uboot_load     	  - download uboot.bin to sd card'