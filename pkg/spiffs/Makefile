PKG_NAME=spiffs
PKG_URL=https://github.com/pellepl/spiffs.git
PKG_VERSION=287148c46587089c4543a21eef2d6e9e14b88364
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

# Disable 'ISO C99 doesn’t support unnamed structs/unions [-Werror=pedantic]'
CFLAGS += -Wno-pedantic

CFLAGS += -Wno-cast-align

# Object names are stored in fixed-size fields that are not necessarily zero
# terminated, which newer GCC versions report when copied with strncpy()
CFLAGS += -Wno-stringop-truncation

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
