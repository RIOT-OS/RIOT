PKG_NAME=cmsis-dsp
PKG_URL=https://github.com/gebart/CMSIS-DSP.git
PKG_VERSION=v1.4.5a-riot1

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
