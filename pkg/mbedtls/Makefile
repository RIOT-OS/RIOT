PKG_NAME=mbedtls
PKG_URL=https://github.com/ARMmbed/mbedtls.git
PKG_VERSION=v2.26.0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKGDIRBASE)/mbedtls/library/ \
			  -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
