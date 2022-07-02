# 构建文件。

.DEFAULT_GOAL := all

# 普通文件构建目标路径。
TARGET = ../targets/objs

# 系统映像构建目标路径。
YROSTARGET = ../targets/YurongOS

# 递归构建目录。
DIRS = boot kernel
.PHONY: $(DIRS)
$(DIRS):
	make --directory=$@


# 部署文件。
# 硬盘映像格式（扇区从1开始编号）：
#   第 [1, 1] 扇区（512B）：boot
#   第 [2, 5] 扇区（2KB）：kernel_loader
.PHONY: deploy
deploy: $(DIRS)
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

$(YROSTARGET)/disk.vmdk: $(YROSTARGET)/disk.img
	qemu-img convert -pO vmdk $< $@

# 将 bochs 格式硬盘转换成 vmdk 格式，以便在 VMware 启动。
.PHONY: vmdk
vmdk: $(YROSTARGET)/disk.vmdk



.PHONY: all
all: deploy
