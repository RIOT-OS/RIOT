PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=6e94414f9c3ad9b77c4635a0ca9e796752a205f0
PKG_LICENSE=Apache-2.0

.PHONY: all

CFLAGS += -Wno-implicit-fallthrough

# disable warnings when compiling with LLVM for board native
ifeq ($(TOOLCHAIN)_$(BOARD),llvm_native)
  CFLAGS += -Wno-macro-redefined -Wno-gnu-folding-constant
endif

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.jerryscript

clean::
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.jerryscript clean


include $(RIOTBASE)/pkg/pkg.mk
