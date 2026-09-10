PKG_NAME = picohttpparser
PKG_URL = https://github.com/h2o/picohttpparser
# v1.2
PKG_VERSION = 539bb9f510f5aefc8efda53aaf822a21ecae2832
PKG_LICENSE = MIT

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base SRC=picohttpparser.c
