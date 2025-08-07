PKG_NAME=qdsa
PKG_URL=https://github.com/RIOT-OS/qdsa.git
PKG_VERSION=3fa25ffa3971000fe4a3e3b42340c40c8d79f2a2
# Not properly licensed, public domain is not a license, cf.
# https://wiki.spdx.org/view/Legal_Team/Decisions/Dealing_with_Public_Domain_within_SPDX_Files
# The license field has therefore been left empty.
PKG_LICENSE=

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(patsubst qdsa_impl_%,%,$(filter qdsa_impl_%,$(USEMODULE)))
