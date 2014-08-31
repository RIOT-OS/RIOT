PKG_NAME = tlsf
PKG_VERSION = 3.0
PKG_FILE = tlsf-$(PKG_VERSION).zip
PKG_URL = http://tlsf.baisoku.org/$(PKG_FILE)

.PHONY: all clean distclean

all: $(BINDIR)$(PKG_NAME).a

$(BINDIR)$(PKG_NAME).a: $(BINDIR)$(PKG_NAME)-src/Makefile
	$(AD)make -C $(<D)

$(BINDIR)$(PKG_NAME)-src/Makefile: $(CURDIR)/$(PKG_FILE) $(CURDIR)/patch.txt
	@rm -rf $(@D)
	@mkdir -p $(@D)
	$(AD)cd $(@D) && $(UNZIP_HERE) $(CURDIR)/$(PKG_FILE)
	$(AD)cd $(@D) && patch --binary -p0 -N -i $(CURDIR)/patch.txt

$(CURDIR)/$(PKG_FILE):
	$(AD)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)

clean::
	rm -rf $(BINDIR)$(PKG_NAME)-src/

distclean:: clean
	rm -f $(CURDIR)/$(PKG_FILE)

Makefile.include:
	@true
