PKG_NAME=desvirt
PKG_URL=https://github.com/des-testbed/desvirt.git
PKG_VERSION=master
PKG_DIR=$(CURDIR)/$(PKG_NAME)

.PHONY: all clean patch distclean desvirtdefine

all: clean $(PKG_NAME) patch desvirtdefine

patch:
	cd "$(PKG_DIR)" && git am --ignore-whitespace "$(CURDIR)"/*.patch

desvirtdefine: patch
	$(foreach topology,$(shell ls $(PKG_NAME)/.desvirt/*.xml), \
		cd $(PKG_NAME) && \
		./vnet --define --name "$(basename $(notdir $(topology)))";)

$(PKG_NAME):
	# Get $(PKG_VERSION) of package from $(PKG_URL)
	$(if $(wildcard $(PKG_NAME)),cd $(CURDIR)/$(PKG_NAME) && \
		git clean -x -f && \
		git reset --hard $(PKG_VERSION) \
		, git clone $(PKG_URL) $(PKG_NAME) && \
		cd $(PKG_NAME) && \
		git reset --hard $(PKG_VERSION))

clean::
	# Reset package to checkout state.
	$(if $(wildcard $(PKG_NAME)),cd $(CURDIR)/$(PKG_NAME) && \
		git clean -x -f && \
		git reset --hard $(PKG_VERSION) \
		, )

distclean::
	rm -rf $(CURDIR)/$(PKG_NAME)
