# 顶层构建文件。

include makefile.inc

.DEFAULT_GOAL := all

# 普通文件构建目标路径。
TARGET = ../build/objs

ASM_DUMP_TARGET = ../build/asm-dump

# 系统映像构建目标路径。
YROS_TARGET = ../build/YurongOS

# 递归构建目录。
DIRS = boot misc lib crt machine memory interrupt task
.PHONY: $(DIRS)
$(DIRS):
	$(MAKE) --directory=$@


# 注意：必须将 kernel_entry 放在开头，不然地址无法正常跳转。
KERNEL_OBJS = $(TARGET)/misc/kernel_entry.o \
			  $(TARGET)/misc/io.o \
			  $(TARGET)/misc/PerCpuCargo.o \
			  $(TARGET)/crt/CRT.o \
			  $(TARGET)/misc/main.o \
			  $(TARGET)/lib/string.o \
			  $(TARGET)/lib/stdio.o \
			  $(TARGET)/lib/syscalls.o \
			  $(TARGET)/lib/LinkedList.o \
			  $(TARGET)/machine/Machine.o \
			  $(TARGET)/machine/GlobalDescriptorTable.o \
			  $(TARGET)/interrupt/InterruptDescriptorTable.o \
			  $(TARGET)/interrupt/InterruptHandlers.o \
			  $(TARGET)/interrupt/KeyboardInterrupt.o \
			  $(TARGET)/interrupt/ClockInterrupt.o \
			  $(TARGET)/interrupt/PageFaultException.o \
			  $(TARGET)/interrupt/SystemCall.o \
			  $(TARGET)/interrupt/InterruptExit.o \
			  $(TARGET)/machine/Cmos.o \
			  $(TARGET)/memory/MemoryManager.o \
			  $(TARGET)/memory/FreeMemoryManager.o \
			  $(TARGET)/memory/ArenaMemoryManager.o \
			  $(TARGET)/memory/KernelMemoryAllocator.o \
			  $(TARGET)/task/TaskManager.o \
			  $(TARGET)/task/TaskManagerSwitchTo.o \
			  $(TARGET)/task/IdleTask.o \

# 构建内核。
.PHONY: yros_kernel
yros_kernel: $(DIRS)
	$(shell mkdir -p $(dir $@))
	$(shell mkdir -p $(ASM_DUMP_TARGET))
	ld -m elf_x86_64 -static $(KERNEL_OBJS) -o $(TARGET)/kernel.exe -T kernel.link.ld 
	objcopy -O binary $(TARGET)/kernel.exe $(TARGET)/kernel.bin
	objdump -d $(TARGET)/kernel.exe > $(ASM_DUMP_TARGET)/kernel.exe.dump.text.asm
	objdump -D $(TARGET)/kernel.exe > $(ASM_DUMP_TARGET)/kernel.exe.dump.full.asm

# 部署文件。
.PHONY: deploy
deploy: yros_kernel
	$(shell mkdir -p $(YROS_TARGET))
	yes | bximage -q -hd=256 -func=create -sectsize=512 -imgmode=flat \
		$(YROS_TARGET)/disk.img
	dd if=$(TARGET)/boot/boot.bin of=$(YROS_TARGET)/disk.img \
		bs=512 count=1 conv=notrunc
	dd if=$(TARGET)/boot/kernel_loader.bin of=$(YROS_TARGET)/disk.img \
		bs=512 count=2 seek=2 conv=notrunc
	dd if=$(TARGET)/kernel.bin of=$(YROS_TARGET)/disk.img \
		bs=512 count=1996 seek=4 conv=notrunc
	cp ../tools/bochsrc $(YROS_TARGET)/


# 清空构建内容。
.PHONY: clean
clean:
	rm -rf $(TARGET)
	rm -rf $(YROS_TARGET)
	rm -rf $(ASM_DUMP_TARGET)

# 构建并启动 bochs
.PHONY: bochs
bochs: deploy
	cd $(YROS_TARGET) && rm -f ./disk.img.lock && bochs -q -f ./bochsrc

.PHONY: bochsg
bochsg: deploy
	cd $(YROS_TARGET) && bochs-gdb -q -f ./bochsrc.gdb

QEMU := qemu-system-x86_64 
QEMU += -m 8G
QEMU += -cpu Icelake-Server 
QEMU += -audiodev pa,id=hda 
QEMU += -machine pcspk-audiodev=hda 
QEMU += -rtc base=localtime 
QEMU += -d cpu_reset -D ./qemu.log 

QEMU_DISK := -boot c -drive file=$(YROS_TARGET)/disk.img,if=ide,index=0,media=disk,format=raw

# qemu。
.PHONY: qemu
qemu: deploy
	$(QEMU) $(QEMU_DISK)

.PHONY: qemug
qemug: deploy
	$(QEMU) $(QEMU_DISK) -s -S

# vmware 硬盘。
$(YROS_TARGET)/disk.vmdk: deploy
	qemu-img convert -O vmdk $(YROS_TARGET)/disk.img $@

# 将 bochs 格式硬盘转换成 vmdk 格式，以便在 VMware 启动。
.PHONY: vmdk
vmdk: $(YROS_TARGET)/disk.vmdk


.PHONY: all
all: deploy
