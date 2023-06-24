# !* ----------------------------------------- *! #
# !*                                           *! #
# !*          this file is deprecated.         *! #
# !*        use CMakeLists.txt instead.        *! #
# !*                                           *! #
# !* ----------------------------------------- *! #

# 操作库构建文件。

.DEFAULT_GOAL := all

include ../makefile.inc

TARGET = ../../build/objs/lib

CPP_SOURCE = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, $(TARGET)/%.o, $(CPP_SOURCE))

$(TARGET)/%.o: %.cpp
	$(shell mkdir -p $(dir $@))
	$(CPP) $(CFLAGS) -I../ -c -o $@ $<

.PHONY: all
all: $(OBJS)
