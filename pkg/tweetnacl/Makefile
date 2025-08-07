PKG_NAME=tweetnacl
PKG_URL=https://github.com/RIOT-OS/tweetnacl
PKG_VERSION=7ea05c7098a16c87fa66e9166ce301666f3f2623
# Not properly licensed, public domain is not a license, cf.
# https://wiki.spdx.org/view/Legal_Team/Decisions/Dealing_with_Public_Domain_within_SPDX_Files
# The license field has therefore been left empty.
PKG_LICENSE=

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
