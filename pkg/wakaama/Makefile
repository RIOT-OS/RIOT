PKG_NAME=wakaama
PKG_URL=https://github.com/eclipse/wakaama.git
PKG_VERSION=da74b3c91570b9716fbb424e90935806b2b29814
PKG_LICENSE=EDL-1.0,EPL-1.0

include $(RIOTBASE)/pkg/pkg.mk

# some variable seem uninitialized to gcc with -Og but are not
# https://gcc.gnu.org/bugzilla/buglist.cgi?quicksearch=may%20be%20used%20uninitialized
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=42145
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90710#c1
CFLAGS += -Wno-maybe-uninitialized

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/core -f $(RIOTBASE)/Makefile.base MODULE=wakaama_core
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/core/er-coap-13 -f $(RIOTBASE)/Makefile.base MODULE=wakaama_core_coap13
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/examples/client -f $(CURDIR)/wakaama_client.mk
