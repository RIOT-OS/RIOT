PKG_NAME=micro-ecc
PKG_URL=https://github.com/kmackay/micro-ecc.git
PKG_VERSION=b6c0cdbe7d20af48b0c2a909a66ff00b093d1542
PKG_DIR=$(CURDIR)
PKG_BUILDDIR=$(BINDIR)/pkg/$(PKG_NAME)

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include
endif

MODULE:=$(shell basename $(CURDIR))

.PHONY: all clean patch reset

all: patch
	make -C $(PKG_BUILDDIR)
	make $(BINDIR)$(MODULE).a

patch: $(PKG_BUILDDIR)/Makefile

$(PKG_BUILDDIR)/Makefile: $(PKG_BUILDDIR)
	cd $(PKG_BUILDDIR) && \
	    for patch in $(PKG_DIR)/[0-9][0-9][0-9][0-9]*.patch; do git am "$${patch}" || { git am --abort; exit 1; }; done
	touch $(PKG_BUILDDIR)/Makefile

$(PKG_BUILDDIR):
	mkdir -p $(BINDIR)/pkg && \
	git clone $(PKG_URL) $@ && \
		cd $@ && git reset --hard $(PKG_VERSION)

clean::
	rm -Rf $(PKG_BUILDDIR)

distclean::
	rm -rf $(PKG_BUILDDIR)

#$(BINDIR)$(MODULE).a: $(BINDIR)micro-ecc_*.a
#	mkdir -p $(BINDIR)$(MODULE); cd $(BINDIR)$(MODULE); for var in $?; do ar -x $$var; done; ar -r -c -s $(BINDIR)$(MODULE).a *.o
