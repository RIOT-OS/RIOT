PKG_NAME=bme680_driver
PKG_URL=https://github.com/BoschSensortec/BME680_driver
PKG_VERSION=63bb5336db4659519860832be2738c685133aa33
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	@cp Makefile.$(PKG_NAME) $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
