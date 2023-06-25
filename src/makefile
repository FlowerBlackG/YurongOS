# 顶层构建文件。
#
# 2023年6月24日
# ---------------
# makefile 不再亲自进行构建。
# makefile 通过调用 cmake 完成构建。
# makefile 仍负责完成 qemu 启动等任务。
#

.DEFAULT_GOAL := all

# 准备。生成 cmake 构建文件。
.PHONY: prepare
prepare:
	cd ../ && mkdir -p build && cd build \
	&& cmake -G"Unix Makefiles" ../src

# 通过调用 cmake 完成构建。具体构建命令在 CMakeLists.txt 内。
.PHONY: build
build: prepare
	cd ../build && cmake --build .


TARGET_DIR := ../target
OBJS_DIR := $(TARGET_DIR)/objs
DISK_IMG := $(TARGET_DIR)/disk.img
.PHONY: deploy
deploy: build
	yes | bximage -q -hd=256 -func=create \
		-sectsize=512 -imgmode=flat \
		$(DISK_IMG)
	dd if=$(OBJS_DIR)/boot/boot.bin of=$(DISK_IMG) \
		bs=512 count=1 conv=notrunc
	dd if=$(OBJS_DIR)/boot/kernel_loader.bin of=$(DISK_IMG) \
		bs=512 count=2 seek=2 conv=notrunc
	dd if=$(OBJS_DIR)/kernel.bin of=$(DISK_IMG) \
		bs=512 count=1996 seek=4 conv=notrunc


.PHONY: all
all: deploy


.PHONY: clean
clean:
	rm -rf ../target
	rm -rf ../build



QEMU := qemu-system-x86_64 
QEMU += -m 8G
QEMU += -cpu Icelake-Server
QEMU += -audiodev pa,id=hda 
QEMU += -machine pcspk-audiodev=hda 
QEMU += -rtc base=localtime 
QEMU += -d cpu_reset -D $(TARGET_DIR)/qemu.log 


QEMU += -chardev stdio,mux=on,id=com1
QEMU += -serial chardev:com1 # 串口1

QEMU_DISK := -boot c -drive file=$(DISK_IMG),if=ide,index=0,media=disk,format=raw


.PHONY: qemu
qemu: deploy
	$(QEMU) $(QEMU_DISK)

# 使用 qemu 启动，且开启 gdb 服务器（用于调试）。
.PHONY: qemug
qemug: deploy
	$(QEMU) $(QEMU_DISK) -s -S

# 在 qemug 的基础上，以守护进程方式启动（适用于 idea 系列 ide）。
.PHONY: qemugd
qemugd: deploy
	$(QEMU) $(QEMU_DISK) -s -S -daemonize

# vmware 硬盘。
$(TARGET_DIR)/disk.vmdk: deploy
	qemu-img convert -O vmdk $(DISK_IMG) $@

# 将 bochs 格式硬盘转换成 vmdk 格式，以便在 VMware 启动。
.PHONY: vmdk
vmdk: $(TARGET_DIR)/disk.vmdk



# ------ deprecated ------ #

# 构建并启动 bochs
.PHONY: bochs
bochs: deploy
	cd $(YROS_TARGET) && rm -f ./disk.img.lock && bochs -q -f ./bochsrc

.PHONY: bochsg
bochsg: deploy
	cd $(YROS_TARGET) && bochs-gdb -q -f ./bochsrc.gdb


