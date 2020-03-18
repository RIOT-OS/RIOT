PKG_NAME     = bossa
PKG_URL      = https://github.com/arduino/BOSSA
PKG_VERSION  = 52e0a4a28721296e64083de7780b30580e0fad16
PKG_LICENSE  = BSD-3-Clause
PKG_BUILDDIR = $(CURDIR)/bin

include $(RIOTBASE)/pkg/pkg.mk

all:
	@echo "[INFO] compiling bossac from source now"
	@env -u CXX COMMON_CXXFLAGS="-std=c++11" $(MAKE) BINDIR=$(PKG_BUILDDIR) -C $(PKG_BUILDDIR) strip-bossac
	@mv $(PKG_BUILDDIR)/bossac $(CURDIR)/bossac

distclean::
	@rm -f $(CURDIR)/bossac
