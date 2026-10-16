PKG_NAME=fatfs
# upstream server is very unreliable, instead host the extracted
# .zip file on GitHub and apply only the upstream patches
PKG_URL=https://github.com/RIOT-OS/FatFS
# r0.15
PKG_VERSION=0015a2f1f797c59d5f33f2777bb645d185e638d3
PKG_LICENSE=BSD-1-Clause

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-overflow

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/source -f $(RIOTBASE)/Makefile.base MODULE=fatfs
