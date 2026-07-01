PKG_NAME := mspdebug
PKG_URL := https://github.com/dlbeer/mspdebug
PKG_VERSION := 985b390ba22f4229aeca9f02e273a54eb4a76a9a
PKG_LICENSE := GPL-2.0-only

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

PKG_SOURCE_DIR = $(CURDIR)/bin
PKG_BUILD_OUT_OF_SOURCE = 0

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/mspdebug

$(CURDIR)/mspdebug:
# Start mspdebug build in a clean environment, so variables set by RIOT's build process
# for cross compiling a specific target platform are reset and mspdebug can
# be built cleanly for the native platform.
	env -i PATH="$(PATH)" TERM="$(TERM)" "$(MAKE)" -C "$(PKG_BUILD_DIR)"
	mv $(PKG_BUILD_DIR)/mspdebug .

clean::
	rm -f $(CURDIR)/mspdebug
