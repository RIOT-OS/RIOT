PKG_NAME=meshcore
PKG_URL=https://github.com/meshcore-dev/MeshCore
# repeater-v1.18.0
PKG_VERSION=81dbae592ffad40b4212bc0b37e82b38da08f4bc
PKG_LICENSE=MIT

# Only the core, its helpers and the ed25519 library are used
PKG_SPARSE_PATHS += src
PKG_SPARSE_PATHS += lib/ed25519

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: meshcore%

all: meshcore meshcore_ed25519 meshcore_helpers

meshcore:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.meshcore

meshcore_ed25519:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/lib/ed25519 -f $(CURDIR)/Makefile.meshcore_ed25519

meshcore_helpers:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/helpers -f $(CURDIR)/Makefile.meshcore_helpers
