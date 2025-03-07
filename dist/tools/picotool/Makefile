PKG_NAME=picotool
PKG_URL=https://github.com/raspberrypi/picotool.git
PKG_VERSION=de8ae5ac334e1126993f72a5c67949712fd1e1a4
PKG_LICENSE=BSD-3-Clause
PKG_BUILD_DIR=$(PKG_SOURCE_DIR)/tools/picotool

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

# Location of the picosdk package
PICOSDK_DIR = ../../../picosdk
PICO_SDK_PATH ?= $(PICOSDK_DIR)/picosdk

USEPKG += picosdk

PKG_SOURCE_DIR = $(CURDIR)/source

# Include pkg.mk after setting RIOTBASE otherwise it is not found when this
# Makefile is called stand-alone
include $(RIOTBASE)/pkg/pkg.mk

# Create a dependency on the picosdk build
$(CURDIR)/picotool: $(PKG_PREPARED) $(PICOSDK_DIR)/.prepared
	echo "[INFO] compiling picotool from source now"
	cd $(PKG_SOURCE_DIR) && cmake -B build -S . -DPICO_SDK_PATH=$(PICO_SDK_PATH) -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
	cd $(PKG_SOURCE_DIR)/build && $(MAKE)
	cp $(PKG_SOURCE_DIR)/build/picotool $(CURDIR)/picotool

# Create a phony target to ensure picosdk is built
.PHONY: picosdk
picosdk: $(PICOSDK_DIR)/.prepared

$(PICOSDK_DIR)/.prepared:
	$(MAKE) -C $(RIOTBASE)/dist/tools//picosdk

all: $(CURDIR)/picotool
