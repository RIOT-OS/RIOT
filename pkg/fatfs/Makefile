PKG_NAME=fatfs
# upstream server is very unreliable, instead host the extracted
# .zip file on GitHub and apply only the upstream patches
PKG_URL=https://github.com/RIOT-OS/FatFS
PKG_VERSION=d8db1de45234d7bcdb4c61e3a3d8af3c9cfb7819 # R0.14 p2
PKG_LICENSE=BSD-1-Clause

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-overflow

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/source -f $(RIOTBASE)/Makefile.base MODULE=fatfs
