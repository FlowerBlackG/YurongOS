# !* ----------------------------------------- *! #
# !*                                           *! #
# !*          this file is deprecated.         *! #
# !*        use CMakeLists.txt instead.        *! #
# !*                                           *! #
# !* ----------------------------------------- *! #

# CRT 构建文件。

.DEFAULT_GOAL := all
include ../makefile.inc
TARGET = ../../build/objs/crt

OBJS = $(TARGET)/CRT.o

$(TARGET)/%.o: %.cpp
	$(shell mkdir -p $(dir $@))
	$(CPP) $(CFLAGS) -I../ -c -o $@ $<

.PHONY: all
all: $(OBJS)
