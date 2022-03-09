PKG_NAME=fatfs
# upstream server is very unreliable, instead host the extracted
# .zip file on GitHub and apply only the upstream patches
PKG_URL=https://github.com/RIOT-OS/FatFS
PKG_VERSION=246c97b1eb7f1900eb00d5ed7e3bf4cdb4558039 # r0.14b
PKG_LICENSE=BSD-1-Clause

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-overflow

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/source -f $(RIOTBASE)/Makefile.base MODULE=fatfs
