PKG_NAME=micro-ecc
PKG_URL=https://github.com/kmackay/micro-ecc.git
PKG_VERSION=b6c0cdbe7d20af48b0c2a909a66ff00b093d1542

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include
endif

.PHONY: all

all: git-download
	make -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
