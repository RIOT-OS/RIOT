PKG_NAME=picosdk
PKG_URL=https://github.com/raspberrypi/pico-sdk.git
PKG_VERSION=bddd20f928ce76142793bef434d4f75f4af6e433
PKG_LICENSE=BSD-3-Clause
PKG_BUILD_DIR=$(PKG_SOURCE_DIR)/tools/picosdk
PKG_SOURCE_DIR = $(CURDIR)/picosdk

RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/picosdk

$(CURDIR)/picosdk: $(PKG_PREPARED)
	echo "[INFO] updating submodules"
	cd $(PKG_SOURCE_DIR) && git submodule update --init lib/mbedtls
