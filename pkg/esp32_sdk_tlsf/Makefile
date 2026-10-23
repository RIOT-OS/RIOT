PKG_NAME=esp32_sdk_tlsf
PKG_URL=https://github.com/espressif/tlsf
PKG_VERSION=2867f6883a12920b1969ff9624c0ab0e4185c2ce
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
