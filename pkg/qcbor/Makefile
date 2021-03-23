PKG_NAME    = qcbor
PKG_URL     = https://github.com/laurencelundblade/QCBOR
PKG_VERSION = 7e8effa94174c33e4acaf737adf6aef16c092c49 # Commit on Mar 9, 2020
PKG_LICENSE = BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

# some variable seem uninitialized to gcc with -Og but are not
# https://gcc.gnu.org/bugzilla/buglist.cgi?quicksearch=may%20be%20used%20uninitialized
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=42145
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90710#c1
CFLAGS += -Wno-maybe-uninitialized

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
