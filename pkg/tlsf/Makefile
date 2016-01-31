PKG_NAME = tlsf
PKG_VERSION = 3.0
PKG_FILE = tlsf-$(PKG_VERSION).zip
PKG_URL = http://download.riot-os.org/$(PKG_FILE)
PKG_DIR=$(CURDIR)/$(PKG_NAME)

.PHONY: all clean distclean

all: $(PKG_DIR)/$(PKG_NAME).a

$(PKG_DIR)/$(PKG_NAME).a: $(PKG_DIR)/Makefile
	$(AD)make -C $(<D)

$(PKG_DIR)/Makefile: $(CURDIR)/$(PKG_FILE) $(CURDIR)/patch.txt
	@rm -rf $(@D)
	@mkdir -p $(@D)
	$(AD)cd $(@D) && $(UNZIP_HERE) $(CURDIR)/$(PKG_FILE)
	$(AD)cd $(@D) && patch --binary -p0 -N -i $(CURDIR)/patch.txt

$(CURDIR)/$(PKG_FILE):
	$(AD)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)

clean::
	rm -rf $(PKG_DIR)/

distclean:: clean
	rm -f $(CURDIR)/$(PKG_FILE)

Makefile.include:
	@true
