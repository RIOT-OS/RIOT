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

GCC_VERSION := $(shell command -v $(CC) > /dev/null && $(CC) -dumpversion | cut -d . -f 1)

# -fmacro-prefix-map requires GCC 8
ifneq (8, $(firstword $(shell echo 8 $(GCC_VERSION) | tr ' ' '\n' | sort -n)))
  OPTIONAL_CFLAGS_BLACKLIST += -fmacro-prefix-map=$(RIOTBASE)/=
endif

# We use GDB for debugging
include $(RIOTMAKE)/tools/gdb.inc.mk

# Data address spaces starts at zero for all supported architectures. This fixes
# compilation at least on MSP430 and AVR.
# See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105523
ifneq (,$(filter $(GCC_VERSION),12 13))
  CFLAGS += --param=min-pagesize=0
endif
