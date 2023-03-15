PKG_NAME=nanors
PKG_URL=https://github.com/sleepybishop/nanors.git
PKG_VERSION=e9e242e98e27037830490b2a752895ca68f75f8b
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

# disable large look-up tables
CFLAGS += -DOBLAS_TINY

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
