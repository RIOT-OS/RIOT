PKG_NAME=bitlash
PKG_URL=https://github.com/d00616/bitlash.git
PKG_VERSION=0069f00606c177fdb7311dc92d64bd47b8d6fa29

MODULE:=$(shell basename $(CURDIR))

.PHONY: all clean patch reset

all: patch
	make -C $(CURDIR)/$(PKG_NAME)

patch: $(CURDIR)/$(PKG_NAME)/Makefile

$(CURDIR)/$(PKG_NAME)/Makefile: $(CURDIR)/$(PKG_NAME)
	$(foreach patch,$(shell ls [0-9][0-9][0-9][0-9]*.patch),cd "$<" && git am "../$(patch)" || { git am --abort; exit 1; };)

$(CURDIR)/$(PKG_NAME):
	test -d $(PKG_NAME) || \
	  git clone $(PKG_URL) $@ && \
	  cd $@ && git reset --hard $(PKG_VERSION)

clean::
	# Reset package to checkout state.
#	cd $(CURDIR)/$(PKG_NAME) || true && \
#		git clean -x -f && \
#		git reset --hard $(PKG_VERSION)

distclean::
	rm -rf $(CURDIR)/$(PKG_NAME)

#$(BINDIR)$(MODULE).a: $(BINDIR)bitlash_*.a
#	mkdir -p $(BINDIR)$(MODULE); cd $(BINDIR)$(MODULE); for var in $?; do ar -x $$var; done; ar -r -c -s $(BINDIR)$(MODULE).a *.o
