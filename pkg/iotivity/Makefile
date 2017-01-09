PKG_NAME=iotivity-constrained
PKG_URL=https://github.com/iotivity/iotivity-constrained
PKG_VERSION=96b50da1762d016790ac7fa516d95a0252bb2f72
PKG_LICENSE=Apache-2.0

PKG_LIB=$(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

MODULE_MAKEFILE := $(PKG_DIR)/Makefile.module

export PKG_DIR
export PKG_LIB
export PKG_NAME

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: git-download submodules-download copy_makefile
	@"$(MAKE)" -BC $(PKG_BUILDDIR) lib-iotivity.a
	@cp $(PKG_BUILDDIR)/lib-iotivity.a ${BINDIR}/iotivity.a

copy_makefile:
	@cp ./Makefile.tomove $(PKG_LIB)/Makefile


submodules-download:
	@git -C $(PKG_LIB) submodule update --init

include $(RIOTBASE)/pkg/pkg.mk

clean:: copy_makefile
	@"$(MAKE)" -BC $(PKG_BUILDDIR) clean
