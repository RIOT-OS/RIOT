PKG_NAME=hacl
# TODO: switch to https://github.com/project-everest/hacl-star
# The mirror was promoted from backup to main URL because upstream was deleted
PKG_URL=https://github.com/RIOT-OS-pkgmirror/hacl-c_archived
PKG_VERSION=aac05f5094fc92569169d5a2af54c12387160634
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -DKRML_NOUINT128 -Wno-unused-parameter -Wno-array-parameter

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
