PKG_NAME=fatfs
# upstream server is very unreliable, instead host the extracted
# .zip file on GitHub and apply only the upstream patches
PKG_URL=https://github.com/RIOT-OS/FatFS
PKG_VERSION=fb99d00924fd17c2d9c0789510852c2c286403ab # R0.14a
PKG_LICENSE=BSD-1-Clause

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-overflow

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/source -f $(RIOTBASE)/Makefile.base MODULE=fatfs
