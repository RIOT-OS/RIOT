PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=bd574956e3702bc5b0512ad835b1df896cf00626
PKG_LICENSE=Apache-2.0

.PHONY: all

CFLAGS += -Wno-implicit-fallthrough

all: git-download
	@cp Makefile.jerryscript $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
