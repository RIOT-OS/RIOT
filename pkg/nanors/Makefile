PKG_NAME=nanors
PKG_URL=https://github.com/sleepybishop/nanors.git
PKG_VERSION=389007b64a66f1c0c38c13bc510da1173cfe6097
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

# disable large look-up tables
CFLAGS += -DOBLAS_TINY

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
