PKG_NAME     := libfixmath
PKG_VERSION  := ad9ed940e57d43432b276e95aee6ca9df6f23ccd
PKG_URL      := https://github.com/PetteriAimonen/libfixmath
PKG_LICENSE  := MIT

.PHONY: all

all: git-download
	$(AD)$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
