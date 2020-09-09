CC         = $(PREFIX)gcc
CXX        = $(PREFIX)g++
CCAS      ?= $(CC)
ifeq ($(LTO),1)
  AR         = $(PREFIX)gcc-ar
  RANLIB     = $(PREFIX)gcc-ranlib
else
  AR         = $(PREFIX)ar
  RANLIB     = $(PREFIX)ranlib
endif
AS         = $(PREFIX)as
NM         = $(PREFIX)nm
LINK       = $(PREFIX)gcc
LINKXX     = $(PREFIX)g++
SIZE       = $(PREFIX)size
_OBJCOPY          := $(shell command -v $(PREFIX)objcopy || command -v gobjcopy || command -v objcopy)
OBJCOPY   ?= $(_OBJCOPY)
ifeq ($(OBJCOPY),)
  $(warning objcopy not found. Hex file will not be created.)
  OBJCOPY    = true
endif
# Default to the native (g)objdump, helps when using toolchain from docker
_OBJDUMP         := $(or $(shell command -v $(PREFIX)objdump || command -v gobjdump),objdump)
OBJDUMP   ?= $(_OBJDUMP)
# We use GDB for debugging
include $(RIOTMAKE)/tools/gdb.inc.mk
