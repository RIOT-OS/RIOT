PKG_NAME=bootterm
PKG_URL=https://github.com/wtarreau/bootterm
# version 0.5
PKG_VERSION=02d6a1a0539f696010770a46d1fd52df53400e69
PKG_LICENSE=MIT

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

PKG_SOURCE_DIR = $(CURDIR)/bin
PKG_BUILD_OUT_OF_SOURCE = 0

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/bt

$(CURDIR)/bt:
# Start bootterm build in a clean environment, so variables set by RIOT's build process
# for cross compiling a specific target platform are reset and bootterm can
# be built cleanly for the native platform.
	env -i PATH="$(PATH)" TERM="$(TERM)" "$(MAKE)" -C "$(PKG_BUILD_DIR)"
	mv $(PKG_BUILD_DIR)/bin/bt .

clean::
	rm -f $(CURDIR)/bt
