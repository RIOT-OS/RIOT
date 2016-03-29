PKG_NAME=cmsis-dsp
PKG_URL=https://github.com/gebart/CMSIS-DSP.git
PKG_VERSION=v1.4.5a-riot1
PKG_DIR=$(CURDIR)/$(PKG_NAME)

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean patch distclean

all: $(PKG_DIR)/Makefile
	$(MAKE) -C $(CURDIR)/$(PKG_NAME)

$(PKG_DIR)/Makefile: $(PKG_DIR)/.git/config
	@

$(PKG_DIR)/.git/config:
	test -d "$(PKG_DIR)" || $(GITCACHE) clone "$(PKG_URL)" "$(PKG_VERSION)" "$(PKG_DIR)"

clean::
	@echo "Cleaning up $(PKG_NAME) package..."
	@-cd "$(PKG_DIR)" 2> /dev/null > /dev/null && \
		git clean -x -f && \
		git am --abort && \
		git reset --hard "$(PKG_VERSION)"

distclean::
	rm -rf "$(PKG_DIR)"
