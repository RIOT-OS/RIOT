PKG_NAME = ficl
PKG_URL = https://github.com/jwsadler58/ficl.git
PKG_VERSION = 68fc76c44d7e1e8aa4fa1faf319fb6591ab3d06b # ficl306-18-g68fc76c
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-array-bounds
CFLAGS += -Wno-char-subscripts
CFLAGS += -Wno-cast-align
CFLAGS += -Wno-format
CFLAGS += -Wno-misleading-indentation
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-strict-aliasing
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-value
CFLAGS += -Wno-unused-variable

#
#   Configuration
#
CFLAGS += -DFICL_WANT_SOFTWORDS=1
CFLAGS += -DFICL_WANT_FILE=0
CFLAGS += -DFICL_WANT_FLOAT=0
CFLAGS += -DFICL_WANT_USER=0
CFLAGS += -DFICL_WANT_LOCALS=0
CFLAGS += -DFICL_WANT_DEBUGGER=0
CFLAGS += -DFICL_WANT_OOP=0
CFLAGS += -DFICL_ROBUST=2
CFLAGS += -DFICL_EXTENDED_PREFIX=0
CFLAGS += -DFICL_UNIT_TEST=0

# includes custom part with ficlCompilePlatform()
CFLAGS += -DFICL_PLATFORM_EXTEND=1

#
# Port (ficlTextOut, ficlMalloc, ficlFree, ficlRealloc), see sysdep.c
#
CFLAGS += -Dlinux

export SRC := \
  dict.c \
  ficl.c \
  fileaccess.c \
  float.c \
  dpmath.c \
  prefix.c \
  search.c \
  softcore.c \
  stack.c \
  sysdep.c \
  tools.c \
  vm.c \
  words.c

all: softcore_c
	$(MAKE) -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base

softcore_c:
	$(MAKE) -C $(PKG_SOURCE_DIR)/softwords softcore.c
	cp $(PKG_SOURCE_DIR)/softwords/softcore.c $(PKG_SOURCE_DIR)/softcore.c
