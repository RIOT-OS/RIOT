PKG_NAME=cmsis-dsp
PKG_URL=https://github.com/gebart/CMSIS-DSP.git
PKG_VERSION=v1.4.5a-riot2
PKG_LICENSE=BSD-3-Clause

CFLAGS += -Wno-strict-aliasing

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
