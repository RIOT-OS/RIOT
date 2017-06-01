PKG_NAME = imx6_SDK
PKG_URL  = https://github.com/backenklee/swp-report/raw/master/pkgfiles
PKG_EXT  = .tar.bz2

IMX6_MODULES=muxer_config

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

DIRS=$(CURDIR)/$(patsubst %,%/,$(IMX6_MODULES))
FILES=$(CURDIR)/$(patsubst %,%$(PKG_EXT),$(IMX6_MODULES))

.PHONY: all clean distclean

all: $(DIRS)

$(DIRS): %/ : %.tar.bz2
	tar -xf $^

$(FILES):
	$(AD)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)/$(patsubst $(CURDIR)/%,%,$@)

clean::
	rm -rf $(DIRS)

distclean:: clean
	rm -rf $(FILES)
