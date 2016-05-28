PKG_NAME     := libfixmath
PKG_VERSION  := ad9ed940e57d43432b276e95aee6ca9df6f23ccd
PKG_URL      := git://github.com/PetteriAimonen/libfixmath
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
	$(AD)$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
