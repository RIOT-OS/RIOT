export GDBPREFIX ?= $(PREFIX)
export CC = $(PREFIX)gcc
export CXX = $(PREFIX)g++
ifeq ($(LTO),1)
export AR = $(PREFIX)gcc-ar
else
export AR = $(PREFIX)ar
endif
export AS = $(PREFIX)as
export LINK = $(PREFIX)gcc
export SIZE = $(PREFIX)size
export OBJCOPY = $(shell command -v $(PREFIX)objcopy gobjcopy objcopy | head -n 1)
ifeq ($(OBJCOPY),)
$(warning objcopy not found. Hex file will not be created.)
export OBJCOPY = true
endif
export OBJDUMP = $(PREFIX)objdump
export DBG = $(GDBPREFIX)gdb
