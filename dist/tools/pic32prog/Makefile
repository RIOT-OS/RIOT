PKG_NAME     = pic32prog
PKG_URL      = https://github.com/sergev/pic32prog
PKG_VERSION  = b9f8db3b352804392b02b42475fc42874ac8bf04
PKG_LICENSE  = GPL-2

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

PKG_SOURCE_DIR = $(CURDIR)/bin
PKG_BUILD_OUT_OF_SOURCE = 0
include $(RIOTBASE)/pkg/pkg.mk

# Building it requires some dependencies, on ubuntu:
#
#     sudo apt-get install libusb-dev libusb-1.0-0-dev libudev-dev

all: $(CURDIR)/pic32prog

$(CURDIR)/pic32prog:
	@echo "[INFO] compiling pic32prog from source now"
	@env -i PATH=$(PATH) TERM=$(TERM) $(MAKE) -C $(PKG_BUILD_DIR)
	@mv $(PKG_BUILD_DIR)/pic32prog $(CURDIR)/pic32prog

clean::
	rm -f $(CURDIR)/pic32prog
