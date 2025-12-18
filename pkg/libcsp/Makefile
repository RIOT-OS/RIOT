PKG_NAME=libcsp
PKG_URL=https://github.com/libcsp/libcsp.git
PKG_VERSION=f429e14f71d9f1cb5187eb6db29b7fcfa8cc373b
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-format

IGNORE_MODULES := libcsp_contrib \
                  libcsp_drivers_can \
                  libcsp_shell \
                  libcsp_shell_server \
                  libcsp_sock_udp \
                  #

LIBCSP_MODULES := $(filter-out $(IGNORE_MODULES),$(filter libcsp_%,$(USEMODULE)))

all: $(LIBCSP_MODULES)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/$(PKG_NAME).mk

libcsp_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/interfaces -f $(CURDIR)/$(PKG_NAME)_$*.mk
