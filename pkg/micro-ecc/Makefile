PKG_NAME=micro-ecc
PKG_URL=https://github.com/kmackay/micro-ecc.git
PKG_VERSION=601bd11062c551b108adbb43ba99f199b840777c
PKG_LICENSE=BSD-2-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
