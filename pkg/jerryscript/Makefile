PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=e1af56586cf9beeafbc67172b05cad28eca289cb
PKG_LICENSE=Apache-2.0

.PHONY: all

CFLAGS += -Wno-implicit-fallthrough

# disable warnings when compiling with LLVM for board native
ifeq ($(TOOLCHAIN)_$(BOARD),llvm_native)
export CFLAGS += -Wno-macro-redefined -Wno-gnu-folding-constant
endif

all: git-download
	@cp Makefile.jerryscript $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
