PKG_NAME = picohttpparser
PKG_URL = https://github.com/h2o/picohttpparser
# v1.4
PKG_VERSION = 516cdd3e00b35569dc0edc5f1b74d245cb323e57
PKG_LICENSE = MIT

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base SRC=picohttpparser.c
