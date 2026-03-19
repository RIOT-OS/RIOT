PKG_NAME=tweetnacl
PKG_URL=https://github.com/RIOT-OS/tweetnacl
PKG_VERSION=7ea05c7098a16c87fa66e9166ce301666f3f2623
# Yielded into public domain by the author.
# Proof of public domain status: https://tweetnacl.cr.yp.to/index.html
PKG_LEGAL_STATUS = public-domain
PKG_LICENSE=

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
