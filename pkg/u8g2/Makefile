PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=3500b1056b33999b285387d196c78fe4ab707f79
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all: $(filter u8g2_%,$(filter-out u8g2_csrc%, $(USEMODULE)))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/csrc -f $(CURDIR)/Makefile.$(PKG_NAME)_csrc

u8g2_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/sys/$*/common -f $(CURDIR)/Makefile.$(PKG_NAME)_$*
