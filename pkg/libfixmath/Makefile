PKG_NAME     := libfixmath
PKG_VERSION  := 7f9c966b5c473770dc93940e3e6e5323f3c1ad69
PKG_URL      := https://github.com/PetteriAimonen/libfixmath
PKG_LICENSE  := MIT

include $(RIOTBASE)/pkg/pkg.mk

all: $(filter libfixmath-unittests,$(USEMODULE))
	"$(MAKE)" -C $(PKG_BUILDDIR)/$(PKG_NAME) -f $(CURDIR)/Makefile.$(PKG_NAME)

libfixmath-unittests:
	"$(MAKE)" -C $(PKG_BUILDDIR)/unittests -f $(CURDIR)/Makefile.$(PKG_NAME)-unittests
