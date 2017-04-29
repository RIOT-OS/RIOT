PKG_NAME=cooja
PKG_URL=https://github.com/phiros/cooja.git
PKG_VERSION=849df5936d45ab7728d384a59232e7962edf2cef

.PHONY: all clean patch distclean

all: $(PKG_NAME)

patch: $(PKG_NAME)
	$(if $(wildcard *.patch), \
		$(foreach patch, \
			$(shell ls [0-9][0-9][0-9][0-9]*.patch),cd $(PKG_NAME) && \
			git am "../$(patch)" || { git am --abort; exit 1; };) \
	, )

$(PKG_NAME)::
	# Get $(PKG_VERSION) of package from $(PKG_URL)
	$(if $(wildcard $(PKG_NAME)),cd $(CURDIR)/$(PKG_NAME) && \
                git clean -x -f && \
                git reset --hard $(PKG_VERSION) && \
		cd $(CURDIR)/$(PKG_NAME)/tools/cooja && \
		ant jar \
		, git clone --recursive $(PKG_URL) $(PKG_NAME) && \
                cd $(CURDIR)/$(PKG_NAME) && \
                git reset --hard $(PKG_VERSION) && \
		cd $(CURDIR)/$(PKG_NAME)/tools/cooja && \
		ant jar)

clean::
	# Reset package to checkout state.
	$(if $(wildcard $(PKG_NAME)),cd $(CURDIR)/$(PKG_NAME) && \
		git clean -x -f && \
		git reset --hard $(PKG_VERSION) \
		, )

distclean::
	rm -rf $(CURDIR)/$(PKG_NAME)
