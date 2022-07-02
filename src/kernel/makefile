# 内核构建文件。

.DEFAULT_GOAL := all

include ../makefile.inc

TARGET = ../../targets/objs/kernel

ENTRY_POINT = 0x10000

ASM_SOURCE = $(wildcard *.asm) # 目录下所有 .asm 文件。空格分隔。

# patsubst 为模式替换。
# 将 *.asm 替换为 $(TARGET)/*.bin。第三个参数为空格分隔的待查找串。
ASM_OBJS = $(patsubst %.asm, $(TARGET)/%.o, $(ASM_SOURCE))

OBJS = $(ASM_OBJS) $(TARGET)/kernel.o

$(TARGET)/kernel.exe: $(OBJS)
	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRY_POINT)
	
$(TARGET)/kernel.bin: $(TARGET)/kernel.exe
	objcopy -O binary $< $@

$(TARGET)/%.o: %.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $< -o $@

$(TARGET)/%.o: %.cpp
	$(shell mkdir -p $(dir $@))
	g++ $(CFLAGS) -I../include -c -o $@ $<

.PHONY: all
all: $(TARGET)/kernel.bin
