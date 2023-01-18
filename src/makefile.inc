# makefile 变量定义。

CC = g++
ASM = nasm
MAKE = make

# CFLAGS 编译器参数。
CFLAGS := -Wall -g -nostartfiles 

CFLAGS += -fno-builtin # 不需要 gcc 的内置函数。
CFLAGS += -nostdinc # 不需要 c 标准库。
CFLAGS += -fno-pic # 不需要位置无关的代码（position independent code）。
CFLAGS += -fno-pie # 不需要位置无关的可执行程序。
CFLAGS += -nostdlib # 不需要标准库。
CFLAGS += -fno-stack-protector # 不需要栈保护。
CFLAGS += -fno-threadsafe-statics

CFLAGS += -mno-red-zone 
CFLAGS += -mno-mmx  
CFLAGS += -mno-sse  
CFLAGS += -mno-sse2 

CFLAGS += -mcmodel=large

CFLAGS := $(strip $(CFLAGS))

# -Wall -g -nostartfiles -fno-builtin -nostdinc -fno-pic -fno-pie -nostdlib -fno-stack-protector -fno-threadsafe-statics
