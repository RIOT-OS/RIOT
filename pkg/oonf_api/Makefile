PKG_NAME=oonf_api
PKG_URL=git://olsr.org/oonf_api.git
PKG_VERSION=v0.3.0

ifneq ($(RIOTBOARD),)
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include -I$(RIOTBASE)/sys/posix/pnet/include
endif

MODULE:=$(shell basename $(CURDIR))

.PHONY: all clean patch reset

all: patch
	make -C $(CURDIR)/$(PKG_NAME)
	make $(BINDIR)$(MODULE).a

patch: $(CURDIR)/$(PKG_NAME)/Makefile
	# Dependancy might be changed accordingly though we think the Makefile
	# will be the first thing you want to change
	#
	# Here might not happen anything besides dependancy checks

$(CURDIR)/$(PKG_NAME)/Makefile: $(CURDIR)/$(PKG_NAME)
	# Here you apply your patch.
	$(foreach patch,$(shell ls [0-9][0-9][0-9][0-9]*.patch),cd "$<" && git am "../$(patch)";)

$(CURDIR)/$(PKG_NAME)/:
	git clone $(PKG_URL) $@ && \
		cd $@ && git checkout $(PKG_VERSION)

clean::
	# Reset package to checkout state.
	cd $(CURDIR)/$(PKG_NAME) || true && \
		git clean -x -f && \
		git reset --hard $(PKG_VERSION)

distclean::
	rm -rf $(CURDIR)/$(PKG_NAME)

$(BINDIR)$(MODULE).a: $(BINDIR)oonf_*.a
	mkdir -p $(BINDIR)$(MODULE); cd $(BINDIR)$(MODULE); for var in $?; do ar -x $$var; done; ar -r -c -s $(BINDIR)$(MODULE).a *.o
