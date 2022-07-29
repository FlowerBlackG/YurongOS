# 构建文件。

.DEFAULT_GOAL := all

# 普通文件构建目标路径。
TARGET = ../targets/objs

# 系统映像构建目标路径。
YROSTARGET = ../targets/YurongOS

# 递归构建目录。
DIRS = boot kernel lib crt machine
.PHONY: $(DIRS)
$(DIRS):
	$(MAKE) --directory=$@

# 内核构建目标路径。
KERNEL_TARGET = $(TARGET)/kernel

# 注意：必须将 kernel_entry 放在开头，不然地址无法正常跳转。
KERNEL_OBJS = $(KERNEL_TARGET)/kernel_entry.o \
			  $(KERNEL_TARGET)/io.o \
			  $(KERNEL_TARGET)/kernel.o \
			  $(TARGET)/crt/CRT.o \
			  $(TARGET)/lib/string.o \
			  $(TARGET)/lib/stdio.o \
			  $(TARGET)/machine/Machine.o \
			  $(TARGET)/machine/GlobalDescriptorTable.o \
			  $(TARGET)/machine/InterruptDescriptorTable.o

# 内核进入点。
KERNEL_ENTRY_POINT = 0x10000

# 构建内核。
.PHONY: yros_kernel
yros_kernel: $(DIRS)
	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $(KERNEL_OBJS) -o $(KERNEL_TARGET)/kernel.exe -T kernel.link.ld 
	objcopy -O binary $(KERNEL_TARGET)/kernel.exe $(KERNEL_TARGET)/kernel.bin

# 部署文件。
# 硬盘映像格式（扇区从1开始编号）：
#   第 [1, 1] 扇区（512B）：boot
#   第 [2, 5] 扇区（2KB）：kernel_loader
.PHONY: deploy
deploy: yros_kernel
	$(shell mkdir -p $(YROSTARGET))
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat \
		$(YROSTARGET)/disk.img
	dd if=$(TARGET)/boot/boot.bin of=$(YROSTARGET)/disk.img \
		bs=512 count=1 conv=notrunc
	dd if=$(TARGET)/boot/kernel_loader.bin of=$(YROSTARGET)/disk.img \
		bs=512 count=4 seek=2 conv=notrunc
	dd if=$(TARGET)/kernel/kernel.bin of=$(YROSTARGET)/disk.img \
		bs=512 count=200 seek=10 conv=notrunc

# 清空构建内容。
.PHONY: clean
clean:
	rm -rf $(TARGET)

# 构建并启动 bochs
.PHONY: bochs
bochs: deploy
	cd $(YROSTARGET) && bochs -q -f ./bochsrc

.PHONY: bochsg
bochsg: deploy
	cd $(YROSTARGET) && bochs-gdb -q -f ./bochsrc.gdb

QEMU := qemu-system-i386 -m 32M -audiodev pa,id=hda -machine pcspk-audiodev=hda -rtc base=localtime
QEMU_DISK := -boot c -drive file=$(YROSTARGET)/disk.img,if=ide,index=0,media=disk,format=raw

# qemu。
.PHONY: qemu
qemu: deploy
	$(QEMU) $(QEMU_DISK)

.PHONY: qemug
qemug: deploy
	$(QEMU) $(QEMU_DISK) -s -S

# vmware 硬盘。
$(YROSTARGET)/disk.vmdk: $(YROSTARGET)/disk.img
	qemu-img convert -pO vmdk $< $@

# 将 bochs 格式硬盘转换成 vmdk 格式，以便在 VMware 启动。
.PHONY: vmdk
vmdk: $(YROSTARGET)/disk.vmdk


.PHONY: all
all: deploy
