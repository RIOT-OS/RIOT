PKG_NAME=wakaama
PKG_URL=git://github.com/eclipse/wakaama.git
PKG_VERSION=69a32cfae39f66fe4eec4cc8d1cd48ced7ad447c
PKG_DIR=$(CURDIR)/$(PKG_NAME)
PKG_TEMP_DIR=$(CURDIR)/temp

.PHONY: all clean patch

all: patch
	"$(MAKE)" -C $(PKG_DIR)

patch: $(PKG_DIR)/Makefile

$(PKG_DIR)/Makefile: $(PKG_TEMP_DIR)/.git/config
	echo 'include $$(RIOTBASE)/Makefile.base' > $(PKG_DIR)/Makefile

$(PKG_TEMP_DIR)/.git/config:
	test -d "$(PKG_TEMP_DIR)" || git clone "$(PKG_URL)" "$(PKG_TEMP_DIR)"; \
		cd "$(PKG_TEMP_DIR)" && git checkout -f "$(PKG_VERSION)"; \
		cd "$(PKG_TEMP_DIR)" && git am --ignore-whitespace "$(CURDIR)"/*.patch; \
		mkdir -p "$(PKG_DIR)" ; \
		cp $(PKG_TEMP_DIR)/core/*.c $(PKG_TEMP_DIR)/core/*.h $(PKG_DIR); \
		cp $(PKG_TEMP_DIR)/core/er-coap-13/*.c $(PKG_TEMP_DIR)/core/er-coap-13/*.h $(PKG_DIR); \
		rm -Rf "$(PKG_TEMP_DIR)"
clean::
	@echo "Cleaning up $(PKG_NAME) package..."
	@cd "$(PKG_DIR)" 2> /dev/null > /dev/null && \
		git clean -x -f && \
		git am --abort && \
		git reset --hard "$(PKG_VERSION)" && \
		$(MAKE) patch || true


distclean::
	rm -rf "$(PKG_DIR)"

Makefile.include:
	@true
