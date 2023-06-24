# !* ----------------------------------------- *! #
# !*                                           *! #
# !*          this file is deprecated.         *! #
# !*        use CMakeLists.txt instead.        *! #
# !*                                           *! #
# !* ----------------------------------------- *! #

# Serial 构建文件。

.DEFAULT_GOAL := all
include ../makefile.inc
TARGET = ../../build/objs/serial

OBJS = $(TARGET)/Serial.o

$(TARGET)/%.o: %.cpp
	$(shell mkdir -p $(dir $@))
	$(CPP) $(CFLAGS) -I../ -c -o $@ $<

.PHONY: all
all: $(OBJS)
