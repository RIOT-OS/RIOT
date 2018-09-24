PKG_NAME=if_lib
PKG_URL=https://github.com/RIOT-OS/if_lib
PKG_VERSION=27057ddbed904138979f8373ded41a30e4fec15c
PKG_LICENSE=MIT
PKG_BUILDDIR=$(CURDIR)/if_lib

.PHONY: all

include $(RIOTBASE)/pkg/pkg.mk
