PKG_NAME = tlsf
PKG_VERSION = 3.0
PKG_FILE = tlsf-$(PKG_VERSION).zip
PKG_URL = http://download.riot-os.org/$(PKG_FILE)
PKG_DIR=$(CURDIR)
PKG_BUILDDIR=$(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)
PKG_SRCDIR=$(PKG_BUILDDIR)/src

.PHONY: all clean distclean

all: $(PKG_SRCDIR)/$(PKG_NAME).a

$(PKG_SRCDIR)/$(PKG_NAME).a: $(PKG_SRCDIR)/Makefile
	$(AD)make -C $(<D)

$(PKG_SRCDIR)/Makefile: $(PKG_BUILDDIR)/$(PKG_FILE) $(CURDIR)/patch.txt
	rm -rf $(@D)
	mkdir -p $(@D)
	$(AD)cd $(@D) && $(UNZIP_HERE) $(PKG_BUILDDIR)/$(PKG_FILE)
	$(AD)cd $(@D) && patch --binary -p0 -N -i $(CURDIR)/patch.txt

$(PKG_BUILDDIR)/$(PKG_FILE):
	@mkdir -p $(@D)
	$(AD)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)

clean::
	rm -rf $(PKG_SRCDIR)/

distclean::
	rm -rf $(PKG_BUILDDIR)/
