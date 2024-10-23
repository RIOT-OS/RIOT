PKG_NAME=wakaama
PKG_URL=https://github.com/eclipse/wakaama.git
PKG_VERSION=ee0c98da7495e1ab43358b49e970dbc97f73ce3b
PKG_LICENSE=EDL-1.0,EPL-1.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

# some variable seem uninitialized to gcc with -Og but are not
# https://gcc.gnu.org/bugzilla/buglist.cgi?quicksearch=may%20be%20used%20uninitialized
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=42145
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90710#c1
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -DLWM2M_SUPPORT_TLV

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/core -f $(RIOTBASE)/Makefile.base MODULE=wakaama_core
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/coap/er-coap-13 -f $(RIOTBASE)/Makefile.base MODULE=wakaama_core_coap13
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/coap -f $(RIOTBASE)/Makefile.base MODULE=wakaama_core_coap
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/data -f $(CURDIR)/wakaama_data.mk
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/examples/client -f $(CURDIR)/wakaama_client.mk
