PKG_NAME = tlsf
PKG_VERSION = 3.0
PKG_FILE = tlsf-$(PKG_VERSION).zip
PKG_URL = http://download.riot-os.org/$(PKG_FILE)
PKG_MD5 = 176965d58af525347e582e1630ea9604
PKG_LICENSE = PD
PKG_DIR=$(CURDIR)
PKG_BUILDDIR=$(PKGDIRBASE)/$(PKG_NAME)
PKG_SRCDIR=$(PKG_BUILDDIR)/src

.PHONY: all prepare clean distclean

all: $(PKG_SRCDIR)/$(PKG_NAME).a

prepare: $(PKG_SRCDIR)/Makefile

$(PKG_SRCDIR)/$(PKG_NAME).a: $(PKG_SRCDIR)/Makefile
	$(Q)"$(MAKE)" -C $(<D)

$(PKG_SRCDIR)/Makefile: $(PKG_BUILDDIR)/$(PKG_FILE) $(CURDIR)/patch.txt
	rm -rf $(@D)
	mkdir -p $(@D)
	$(Q)cd $(@D) && $(UNZIP_HERE) $(PKG_BUILDDIR)/$(PKG_FILE)
	$(Q)cd $(@D) && patch --binary -p0 -N -i $(CURDIR)/patch.txt

$(PKG_BUILDDIR)/$(PKG_FILE):
	@mkdir -p $(@D)
	$(Q)$(DLCACHE) $(PKG_URL) $(PKG_MD5) $@

clean::
	rm -rf $(PKG_SRCDIR)/

distclean::
	rm -rf $(PKG_BUILDDIR)/
