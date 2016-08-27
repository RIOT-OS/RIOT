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
export OBJCOPY = $(PREFIX)objcopy
export OBJDUMP = $(PREFIX)objdump
export DBG = $(GDBPREFIX)gdb
