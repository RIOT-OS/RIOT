PKG_NAME = tlsf
PKG_VERSION = 3.0
PKG_FILE = tlsf-$(PKG_VERSION).zip
PKG_URL = http://tlsf.baisoku.org/$(PKG_FILE)

ifeq (, $(DOWNLOAD_TO))
  ifeq (0, $(shell which wget 2>&1 > /dev/null ; echo $$?))
    DOWNLOAD_TO := $(shell which wget) -nv -c -O
  else
    ifeq (0, $(shell which curl 2>&1 > /dev/null ; echo $$?))
      DOWNLOAD_TO := $(shell which curl) -s -o
    else
      $(error "Neither wget nor curl is installed.")
    endif
  endif
endif

ifeq (, $(UNZIP))
  ifeq (0, $(shell which unzip 2>&1 > /dev/null ; echo $$?))
    UNZIP := $(shell which unzip)
  else
    ifeq (0, $(shell which 7z 2>&1 > /dev/null ; echo $$?))
      UNZIP := $(shell which 7z) x
    else
      $(error "Neither unzip nor 7z is installed.")
    endif
  endif
endif

.PHONY: all clean distclean

all: $(BINDIR)$(PKG_NAME).a

$(BINDIR)$(PKG_NAME).a: $(BINDIR)$(PKG_NAME)-src/Makefile
	$(AD)make -C $(<D)

$(BINDIR)$(PKG_NAME)-src/Makefile: $(CURDIR)/$(PKG_FILE) $(CURDIR)/patch.txt
	@rm -rf $(@D)
	@mkdir -p $(@D)
	$(AD)cd $(@D) && $(UNZIP) $(CURDIR)/$(PKG_FILE)
	$(AD)cd $(@D) && patch --binary -p0 -N -i $(CURDIR)/patch.txt

$(CURDIR)/$(PKG_FILE):
	$(AD)$(DOWNLOAD_TO) $@ $(PKG_URL)

clean::
	rm -rf $(BINDIR)$(PKG_NAME)-src/

distclean:: clean
	rm -f $(CURDIR)/$(PKG_FILE)

Makefile.include:
	@true
