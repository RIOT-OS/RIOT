PKG_NAME=mbedtls
PKG_URL=https://github.com/ARMmbed/mbedtls.git
PKG_VERSION=8b3f26a5ac38d4fdccbc5c5366229f3e01dafcc0  # v2.28.0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKGDIRBASE)/mbedtls/library/ \
			  -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
