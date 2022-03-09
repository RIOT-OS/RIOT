PKG_NAME = tiny-asn1
PKG_URL = https://gitlab.com/mtausig/tiny-asn1.git
PKG_VERSION = 7005fcff4706e96b857f257ef94b8518211c9fbc
PKG_LICENSE = LGPL-3

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
