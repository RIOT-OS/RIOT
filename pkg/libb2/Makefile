PKG_NAME    = libb2
PKG_URL     = https://github.com/BLAKE2/libb2
PKG_VERSION = 60ea749837362c226e8501718f505ab138e5c19d # v0.98
PKG_LICENSE = CC0-1.0

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src \
			  -f $(RIOTPKG)/libb2/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
