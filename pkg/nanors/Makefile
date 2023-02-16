PKG_NAME=nanors
PKG_URL=https://github.com/sleepybishop/nanors.git
PKG_VERSION=395e5ada44dd8d5974eaf6bb6b17f23406e3ca72
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

# disable large look-up tables
CFLAGS += -DOBLAS_TINY

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
