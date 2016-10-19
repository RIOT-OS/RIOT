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
ifeq (,$(OBJCOPY))
ifneq (,$(shell command -v $(PREFIX)objcopy 2>/dev/null))
export OBJCOPY = $(PREFIX)objcopy
else
ifneq (,$(shell command -v gobjcopy 2>/dev/null))
export OBJCOPY = gobjcopy
else
export OBJCOPY = objcopy
endif
endif
endif
export OBJDUMP = $(PREFIX)objdump
export DBG = $(GDBPREFIX)gdb
