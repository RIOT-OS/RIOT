PKG_NAME=littlefs2
PKG_URL=https://github.com/littlefs-project/littlefs.git
# v2.9.2
PKG_VERSION=68d28b5114467c897fa3b4d44056f6971626de7c
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# Disable debug printing
ifneq ($(DEVELHELP),1)
  CFLAGS += -DLFS_NO_DEBUG -DLFS_NO_WARN -DLFS_NO_ERROR
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
