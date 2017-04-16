PKG_NAME=contiki
PKG_URL=https://github.com/contiki-os/contiki.git
PKG_VERSION=6fb7dd238e0421849cc3a8bee71d3e4491b2b1a7

.PHONY: all clean patch distclean

all: patch

patch: $(PKG_NAME)
	$(foreach patch,$(shell ls [0-9][0-9][0-9][0-9]*.patch),cd $(PKG_NAME) && git am "../$(patch)" || { git am --abort; exit 1; };)

$(PKG_NAME):
	# Get $(PKG_VERSION) of package from $(PKG_URL) (also fetch submodules)
	$(if $(wildcard $(PKG_NAME)),cd $(CURDIR)/$(PKG_NAME) && \
                git clean -x -f && \
                git reset --hard $(PKG_VERSION) \
                , git clone --recursive $(PKG_URL) $(PKG_NAME) && \
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
