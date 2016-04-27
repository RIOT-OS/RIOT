PKG_NAME=tinydtls
PKG_URL=git://github.com/OlegHahm/tinydtls.git
PKG_VERSION=tinydtls_riot
PKG_DIR=$(CURDIR)/$(PKG_NAME)

.PHONY: all clean patch reset

all: $(PKG_DIR)/.git/config
	"$(MAKE)" -C $(PKG_DIR)

$(PKG_DIR)/.git/config:
	test -d "$(PKG_DIR)" || git clone "$(PKG_URL)" "$(PKG_DIR)"; \
		cd "$(PKG_DIR)" && git checkout -f "$(PKG_VERSION)"

clean::
	@echo "Cleaning up $(PKG_NAME) package..."
	@cd "$(PKG_DIR)" 2> /dev/null > /dev/null && \
		git reset --hard "$(PKG_VERSION)"

distclean::
	rm -rf "$(PKG_DIR)"

Makefile.include:
	@true
