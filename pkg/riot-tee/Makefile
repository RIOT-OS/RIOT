PKG_SOURCE_LOCAL=~/work/riot-tee
PKG_NAME=riot-tee
PKG_URL=https://github.com/Einhornhool/riot-tee
PKG_VERSION=4b79ef4d339ee789d9520ca46c128be46592454e
PKG_LICENSE=GPL-v3.0

include $(RIOTBASE)/pkg/pkg.mk

export DEBUG=1
export NRFXPATH = $(RIOTBASE)/build/pkg/nrfx
export CMSISPATH = $(RIOTBASE)/build/pkg/cmsis/CMSIS
export BUILDDIR = $(PKG_BUILD_DIR)
export TOOLCHAINPATH = /home/lena/apps/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(PKG_SOURCE_DIR)/Makefile
