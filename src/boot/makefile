# !* ----------------------------------------- *! #
# !*                                           *! #
# !*          this file is deprecated.         *! #
# !*        use CMakeLists.txt instead.        *! #
# !*                                           *! #
# !* ----------------------------------------- *! #

# 启动引导构建文件。

include ../makefile.inc

.DEFAULT_GOAL := all

TARGET = ../../build/objs/boot

SOURCE = $(wildcard *.asm) # 目录下所有 .asm 文件。空格分隔。

# patsubst 为模式替换。
# 将 *.asm 替换为 $(TARGET)/*.bin。第三个参数为空格分隔的待查找串。
OBJS = $(patsubst %.asm, $(TARGET)/%.bin, $(SOURCE))

$(TARGET)/%.bin: %.asm
	$(shell mkdir -p $(dir $@))
	$(ASM) -f bin $< -o $@

.PHONY: all
all: $(OBJS)
