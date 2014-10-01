PKG_NAME=desvirt
PKG_URL=https://github.com/des-testbed/desvirt.git
PKG_VERSION=17e50ba2c653adcf3ba9911f0a629a05dc970d24

.PHONY: all clean patch distclean desvirtdefine

all: patch desvirtdefine

patch: $(PKG_NAME)
	$(foreach patch,$(shell ls [0-9][0-9][0-9][0-9]*.patch),cd $(PKG_NAME) && git am "../$(patch)" || { git am --abort; exit 1; };)

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
