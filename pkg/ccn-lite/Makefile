PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=57be9b1985dc501b26f861b24448b54db37f73f1
PKG_DIR=$(CURDIR)/$(PKG_NAME)

ifneq ($(RIOTBOARD),)
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

.PHONY: all clean distclean

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: $(PKG_DIR)/Makefile
	"$(MAKE)" -BC $(PKG_DIR)/src lib-ccn-lite.a
	"$(MAKE)" -BC $(PKG_DIR)/src lib-ccn-lite-utils.a
	cp $(PKG_DIR)/src/lib-ccn-lite.a ${BINDIR}/ccn-lite.a
	cp $(PKG_DIR)/src/lib-ccn-lite-utils.a ${BINDIR}/ccn-lite-utils.a

$(PKG_DIR)/Makefile: $(PKG_DIR)/.git/config

$(PKG_DIR)/.git/config:
	test -d "$(PKG_DIR)" || git clone "$(PKG_URL)" "$(PKG_DIR)"; \
		cd "$(PKG_DIR)" && git fetch && git checkout -f "$(PKG_VERSION)"

clean::
	@echo "Cleaning up CCN-Lite package..."
	@cd "$(PKG_DIR)" 2> /dev/null > /dev/null && \
		git clean -x -f && \
		git reset --hard "$(PKG_VERSION)"

distclean::
	rm -rf "$(PKG_DIR)"

Makefile.include:
	@true
