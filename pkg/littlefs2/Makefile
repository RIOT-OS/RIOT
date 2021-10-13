PKG_NAME=littlefs2
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v2.4
PKG_VERSION=1863dc7883d82bd6ca79faa164b65341064d1c16
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format
# GCC 4.9 bug (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64480)
# used by MIPS
CFLAGS += -Wno-missing-field-initializers

# Disable debug printing
ifneq ($(DEVELHELP),1)
  CFLAGS += -DLFS_NO_DEBUG -DLFS_NO_WARN -DLFS_NO_ERROR
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
