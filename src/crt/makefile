# CRT 构建文件。

.DEFAULT_GOAL := all
include ../makefile.inc
TARGET = ../../targets/objs/crt

OBJS = $(TARGET)/CRT.o

$(TARGET)/%.o: %.cpp
	$(shell mkdir -p $(dir $@))
	g++ $(CFLAGS) -I../include -c -o $@ $<

.PHONY: all
all: $(OBJS)
