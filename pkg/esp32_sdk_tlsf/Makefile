PKG_NAME=esp32_sdk_tlsf
PKG_URL=https://github.com/espressif/tlsf
PKG_VERSION=ba64d198a845df70b481e2c55004521ca643dea6
PKG_LICENSE=BSD

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
