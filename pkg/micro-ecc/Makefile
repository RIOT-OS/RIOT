PKG_NAME=micro-ecc
PKG_URL=https://github.com/kmackay/micro-ecc.git
PKG_VERSION=f3d46f0fb77b42535168a17b4b51802beb124a32

ifneq ($(RIOTBOARD),)
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include
endif

MODULE:=$(shell basename $(CURDIR))

.PHONY: all clean patch reset

all: patch
	make -C $(CURDIR)/$(PKG_NAME)
	make $(BINDIR)$(MODULE).a

patch: $(CURDIR)/$(PKG_NAME)/Makefile

$(CURDIR)/$(PKG_NAME)/Makefile: $(CURDIR)/$(PKG_NAME)
	$(foreach patch,$(shell ls [0-9][0-9][0-9][0-9]*.patch),cd "$<" && git am "../$(patch)" || { git am --abort; exit 1; };)
	touch $(CURDIR)/$(PKG_NAME)/Makefile

$(CURDIR)/$(PKG_NAME):
	test -d $(PKG_NAME) || \
	git clone $(PKG_URL) $@ && \
		cd $@ && git reset --hard $(PKG_VERSION)

clean::
	# Reset package to checkout state.
	cd $(CURDIR)/$(PKG_NAME) || true && \
		git clean -x -f && \
		git reset --hard $(PKG_VERSION)

distclean::
	rm -rf $(CURDIR)/$(PKG_NAME)

#$(BINDIR)$(MODULE).a: $(BINDIR)micro-ecc_*.a
#	mkdir -p $(BINDIR)$(MODULE); cd $(BINDIR)$(MODULE); for var in $?; do ar -x $$var; done; ar -r -c -s $(BINDIR)$(MODULE).a *.o
