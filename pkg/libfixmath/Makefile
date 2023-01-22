PKG_NAME     := libfixmath
PKG_VERSION  := b987044c73dcaba496bb5bc86e69d134cd8790ec
PKG_URL      := https://github.com/PetteriAimonen/libfixmath
PKG_LICENSE  := MIT

include $(RIOTBASE)/pkg/pkg.mk

all: $(filter libfixmath-unittests,$(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(PKG_NAME) -f $(CURDIR)/Makefile.$(PKG_NAME)

libfixmath-unittests:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/tests -f $(CURDIR)/Makefile.$(PKG_NAME)-unittests
