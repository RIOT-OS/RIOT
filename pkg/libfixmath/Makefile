PKG_NAME     := libfixmath
PKG_VERSION  := 24488b16cc0359daada0682a9bd3a11a801d0a01
PKG_URL      := https://github.com/PetteriAimonen/libfixmath
PKG_LICENSE  := MIT

include $(RIOTBASE)/pkg/pkg.mk

all: $(filter libfixmath-unittests,$(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(PKG_NAME) -f $(CURDIR)/Makefile.$(PKG_NAME)

libfixmath-unittests:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/unittests -f $(CURDIR)/Makefile.$(PKG_NAME)-unittests
