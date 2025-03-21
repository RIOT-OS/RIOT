PKG_NAME=doxygen
# URL Format: https://github.com/doxygen/doxygen/releases/download/Release_1_13_2/doxygen-1.13.2.src.tar.gz
PKG_URL=https://github.com/doxygen/doxygen/releases/download/Release_
PKG_EXT=linux.bin.tar.gz
PKG_LICENSE=GPL-2.0

# the PKG_VERSION is defined by the doc/doxygen/Makefile
ifeq (,$(filter clean distclean,$(MAKECMDGOALS)))
  ifeq (,$(PKG_VERSION))
    $(error Doxygen version undefined!)
  endif
endif

.PHONY: clean distclean

all: $(CURDIR)/doxygen

$(CURDIR)/doxygen: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	@echo "[INFO] Unpacking Doxygen..."
	@tar -xf $<
	@cp $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/bin/doxygen $(CURDIR)

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT):
	@echo "[INFO] Downloading Doxygen..."
	@wget -nv -O $@ $(PKG_URL)$(subst .,_,$(PKG_VERSION))/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)

clean distclean::
	rm -rf $(CURDIR)/$(PKG_NAME)*
