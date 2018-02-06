PKG_NAME=tweetnacl
PKG_URL=https://github.com/RIOT-OS/tweetnacl
PKG_VERSION=7ea05c7098a16c87fa66e9166ce301666f3f2623
PKG_LICENSE=PD

.PHONY: all

all: git-download
	@cp $(RIOTBASE)/pkg/tweetnacl/src/* $(PKG_BUILDDIR)
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
