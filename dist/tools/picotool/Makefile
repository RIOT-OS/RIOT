PKG_NAME=picotool
PKG_URL=https://github.com/raspberrypi/picotool.git
# Version 2.2.0-a4
PKG_VERSION=25aa087b2c517b4901874a99536e869d4d27b678
PKG_LICENSE=BSD-3-Clause

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

# Location of the picosdk package
PICOSDK_DIR = $(RIOTBASE)/build/pkg/picosdk

PKG_SOURCE_DIR = $(CURDIR)/source
PKG_BUILD_DIR = $(PKG_SOURCE_DIR)/build

# Include pkg.mk after setting RIOTBASE otherwise it is not found when this
# Makefile is called stand-alone
include $(RIOTBASE)/pkg/pkg.mk

# Create a dependency on the picosdk build
$(CURDIR)/picotool: $(PKG_PREPARED) picosdk
	@echo "[INFO] compiling picotool from source now"
	@cd $(PKG_SOURCE_DIR) && cmake -B build -S . -DPICO_SDK_PATH=$(PICOSDK_DIR) -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
	@cd $(PKG_BUILD_DIR) && env --unset=CC --unset=CXX $(MAKE)
	@cp $(PKG_BUILD_DIR)/picotool $(CURDIR)/picotool

.PHONY: picosdk
picosdk:
	$(MAKE) -C $(RIOTBASE)/pkg/picosdk

all: $(CURDIR)/picotool

clean::
	rm -f $(CURDIR)/picotool
