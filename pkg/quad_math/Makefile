PKG_NAME=       quad_math
PKG_URL=        anoncvs@anoncvs.ca.openbsd.org:/cvs
PKG_VERSION=    -rOPENBSD_5_4
PKG_SRC=        src/lib/libc/quad

ifneq ($(RIOTBOARD),)
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

.PHONY: all clean distclean

all: $(CURDIR)/$(PKG_NAME)/Makefile
	"$(MAKE)" -C $(CURDIR)/$(PKG_NAME)

$(CURDIR)/$(PKG_NAME)/Makefile: $(CURDIR)/$(PKG_NAME)/
	cd $(CURDIR)/$(PKG_NAME) && patch -p0 -N -i ../patch.txt

$(CURDIR)/$(PKG_NAME)/: $(CURDIR)/$(PKG_SRC)/
	rm -rf $@
	mkdir -p $@
	cd $@ && cp -a $^/*.c $^/*.h .

$(CURDIR)/$(PKG_SRC)/:
	rm -rf $@
	cvs -q -z9 -d $(PKG_URL) get $(PKG_VERSION) -P $(PKG_SRC)

clean::
	rm -rf $(CURDIR)/$(PKG_NAME)

distclean:: clean
	rm -rf $(CURDIR)/$(PKG_SRC)
