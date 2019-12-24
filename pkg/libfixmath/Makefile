PKG_NAME     := libfixmath
PKG_VERSION  := 7f9c966b5c473770dc93940e3e6e5323f3c1ad69
PKG_URL      := https://github.com/PetteriAimonen/libfixmath
PKG_LICENSE  := MIT

.PHONY: all

all:
	$(Q)cp $(CURDIR)/Makefile.$(PKG_NAME) $(PKG_BUILDDIR)/$(PKG_NAME)/Makefile
	$(Q)cp $(CURDIR)/Makefile.$(PKG_NAME)-unittests $(PKG_BUILDDIR)/unittests/Makefile

include $(RIOTBASE)/pkg/pkg.mk
