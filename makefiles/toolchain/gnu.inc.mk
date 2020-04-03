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
_OBJCOPY          := $(shell command -v $(PREFIX)objcopy || command -v gobjcopy || command -v objcopy)
export OBJCOPY   ?= $(_OBJCOPY)
ifeq ($(OBJCOPY),)
$(warning objcopy not found. Hex file will not be created.)
export OBJCOPY    = true
endif
# Default to the native (g)objdump, helps when using toolchain from docker
_OBJDUMP         := $(or $(shell command -v $(PREFIX)objdump || command -v gobjdump),objdump)
export OBJDUMP   ?= $(_OBJDUMP)
# We use GDB for debugging
include $(RIOTMAKE)/tools/gdb.inc.mk
