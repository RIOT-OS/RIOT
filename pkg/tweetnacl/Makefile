PKG_NAME = tweetnacl
PKG_VERSION = 20140427
PKG_FILE_C = tweetnacl.c
PKG_FILE_H = tweetnacl.h
PKG_URL = https://tweetnacl.cr.yp.to/
PKG_DIR=$(CURDIR)
PKG_BUILDDIR=$(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean patch distclean

all: patch
	$(MAKE) -C $(PKG_BUILDDIR)
	cp $(PKG_BUILDDIR)/$(PKG_NAME).a $(BINDIR)$(PKG_NAME).a

patch: $(PKG_BUILDDIR)/Makefile
	# Dependancy might be changed accordingly though we think the Makefile
	# will be the first thing you want to change
	#
	# Here might not happen anything besides dependancy checks

$(PKG_BUILDDIR)/Makefile: $(PKG_BUILDDIR)/
	# Here you apply your patch.
	# we skip the return of patch 
	$(AD)cd $(@D) && patch -p0 -N -i $(CURDIR)/patch.txt || exit 0


$(PKG_BUILDDIR)/:
	@mkdir -p $@
	$(AD)$(DOWNLOAD_TO_FILE) $@/$(PKG_FILE_C) $(PKG_URL)$(PKG_VERSION)/$(PKG_FILE_C)
	$(AD)$(DOWNLOAD_TO_FILE) $@/$(PKG_FILE_H) $(PKG_URL)$(PKG_VERSION)/$(PKG_FILE_H)

clean::
	# Reset package to checkout state.
	rm -rf $(PKG_BUILDDIR) && \
		$(MAKE) $(PKG_BUILDDIR)/Makefile

distclean::
	rm -rf $(PKG_BUILDDIR)
