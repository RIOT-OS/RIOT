PKG_NAME=ucglib
PKG_URL=https://github.com/olikraus/ucglib
PKG_VERSION=230f15e3bcd3c84977780e84bd855ac89c1959ee
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: ucglib_sdl

all: $(filter ucglib_sdl,$(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/Makefile.ucglib
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/csrc -f $(CURDIR)/Makefile.ucglib_csrc

ucglib_sdl:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/sys/sdl/dev -f $(CURDIR)/Makefile.ucglib_sdl
