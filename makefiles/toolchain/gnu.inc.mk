export CC         = $(PREFIX)gcc
export CXX        = $(PREFIX)g++
export CCAS      ?= $(CC)
ifeq ($(LTO),1)
export AR         = $(PREFIX)gcc-ar
export RANLIB     = $(PREFIX)gcc-ranlib
else
export AR         = $(PREFIX)ar
export RANLIB     = $(PREFIX)ranlib
endif
export AS         = $(PREFIX)as
export NM         = $(PREFIX)nm
export LINK       = $(PREFIX)gcc
export LINKXX     = $(PREFIX)g++
export SIZE       = $(PREFIX)size
export OBJCOPY   ?= $(shell command -v $(PREFIX)objcopy gobjcopy objcopy | head -n 1)
ifeq ($(OBJCOPY),)
$(warning objcopy not found. Hex file will not be created.)
export OBJCOPY    = true
endif
export OBJDUMP    = $(PREFIX)objdump
# We use GDB for debugging
include $(RIOTMAKE)/tools/gdb.inc.mk
