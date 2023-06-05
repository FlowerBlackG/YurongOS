# 内核构建文件。

.DEFAULT_GOAL := all

include ../makefile.inc

TARGET = ../../build/objs/misc

# 目录下所有 .asm 文件。空格分隔。
ASM_SOURCE = $(wildcard *.asm)
CPP_SOURCE = $(wildcard *.cpp)

# patsubst 为模式替换。
# 将 *.asm 替换为 $(TARGET)/*.bin。第三个参数为空格分隔的待查找串。
ASM_OBJS = $(patsubst %.asm, $(TARGET)/%.o, $(ASM_SOURCE))
CPP_OBJS = $(patsubst %.cpp, $(TARGET)/%.o, $(CPP_SOURCE))

OBJS = $(CPP_OBJS) $(ASM_OBJS)

$(TARGET)/%.o: %.asm
	$(shell mkdir -p $(dir $@))
	$(ASM) -f elf64 $< -o $@

$(TARGET)/%.o: %.cpp
	$(shell mkdir -p $(dir $@))
	$(CPP) $(CFLAGS) -I../ -c -o $@ $<

.PHONY: all
all: $(OBJS)
