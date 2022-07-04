# 操作库构建文件。

.DEFAULT_GOAL := all

include ../makefile.inc

TARGET = ../../targets/objs/lib

CPP_SOURCE = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, $(TARGET)/%.o, $(CPP_SOURCE))

$(OBJS): $(CPP_SOURCE)
	$(shell mkdir -p $(dir $@))
	g++ $(CFLAGS) -I../include -c -o $@ $<

.PHONY: all
all: $(OBJS)
