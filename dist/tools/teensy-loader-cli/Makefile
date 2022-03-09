PKG_NAME=teensy-loader-cli
PKG_LICENSE=GPL-3

# get sources from repository
ifeq ($(OS),Darwin)
  # hacked version to make it work on MACOSX
  PKG_URL=https://github.com/alswl/teensy_loader_cli.git
  PKG_VERSION=9c16bb0add3ba847df5509328ad6bd5bc09d9ecd
else
  PKG_URL=https://github.com/PaulStoffregen/teensy_loader_cli.git
  PKG_VERSION=76921edbdd81ae99b869b104404c16c06b0a266f
endif

PKG_SOURCE_DIR = $(CURDIR)/bin
PKG_BUILD_OUT_OF_SOURCE = 0
include $(RIOTBASE)/pkg/pkg.mk

all:
	env -i PATH=$(PATH) TERM=$(TERM) "$(MAKE)" -C $(PKG_SOURCE_DIR)
	mv $(PKG_SOURCE_DIR)/teensy_loader_cli ./teensy_loader
