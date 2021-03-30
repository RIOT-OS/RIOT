PKG_NAME=hacl
# TODO: switch to https://github.com/project-everest/hacl-star
# backup of last state before upstream was deleted
PKG_URL=https://github.com/benpicco/hacl-c_archived
PKG_VERSION=aac05f5094fc92569169d5a2af54c12387160634
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -DKRML_NOUINT128 -Wno-unused-parameter

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
