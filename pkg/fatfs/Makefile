PKG_NAME     = fatfs
PKG_URL      = elm-chan.org/fsw/ff/arc
PKG_VERSION  = ff12b
PKG_EXT      = zip
PKG_FILE     = $(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
PKG_BUILDDIR = $(PKGDIRBASE)/$(PKG_NAME)
MODULE_MAKEFILE := $(CURDIR)/Makefile.module

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean patch

all: $(PKG_BUILDDIR)/.patched

ffconf_patches += $(CURDIR)/patches/enable_use_lable.patch
ffconf_patches += $(CURDIR)/patches/set_code_page_to_ascii.patch
ffconf_patches += $(CURDIR)/patches/enable_use_mkfs.patch
ffconf_patches += $(CURDIR)/patches/enable_fs_tiny.patch

#if periph_rtc is available use it. Otherwise use static timestamps
ifeq (, $(filter $(periph_rtc), $(FEATURES_PROVIDED)))
ffconf_patches += $(CURDIR)/patches/disable_rtc.patch
FEATURES_REQUIRED += periph_rtc
endif

$(PKG_BUILDDIR)/.patched: $(PKG_BUILDDIR)/.prepared
	@$(foreach p, $(ffconf_patches), patch $(PKG_BUILDDIR)/ffconf.h < $(p);)
	@patch $(PKG_BUILDDIR)/integer.h < $(CURDIR)/patches/remove_int_type.patch
	@patch $(PKG_BUILDDIR)/ff.c < $(CURDIR)/patches/fix_for_guards.patch
	@touch $@

$(PKG_BUILDDIR)/.prepared: $(PKG_BUILDDIR)/.extracted
	@mv $(PKG_BUILDDIR)/src/* $(PKG_BUILDDIR)/
	@rm -rf $(PKG_BUILDDIR)/src
	@rm -rf $(PKG_BUILDDIR)/doc

#this file is only a dummy that should be replaced by a compatible implementation
	@rm $(PKG_BUILDDIR)/diskio.c
	@cp $(MODULE_MAKEFILE) $(PKG_BUILDDIR)/Makefile
	@touch $@

$(PKG_BUILDDIR)/.extracted: $(PKG_BUILDDIR)/$(PKG_FILE)
	@mkdir -p $(@D)
	@cd $(@D) && $(UNZIP_HERE) $(<)
	@touch $@

$(PKG_BUILDDIR)/$(PKG_FILE):
	@mkdir -p $(@D)
	@$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)/$(PKG_VERSION).$(PKG_EXT)

clean::
#delete all files except the downloaded zip file
	@ls -A $(PKG_BUILDDIR)/ | grep -v $(PKG_FILE) | xargs -I % rm -rf $(PKG_BUILDDIR)/%

distclean::
	@rm -rf $(PKG_BUILDDIR)
