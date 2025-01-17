PKG_NAME=nrfx
PKG_URL=https://github.com/NordicSemiconductor/nrfx
# Release 3.6.0
PKG_VERSION=075e9e711383bb3babfae4249fd59c8c8d882f72
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

PSRC = $(PKGDIRBASE)/nrfx/drivers/src

# this will build the nrfx_nfct.c file
# this target needs a different name later on
all:
	$(QQ)"$(MAKE)" -C $(PSRC) -f $(CURDIR)/nrfx_nfct.mk
