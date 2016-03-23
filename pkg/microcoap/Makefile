PKG_NAME=microcoap
PKG_URL=git://github.com/1248/microcoap.git
PKG_VERSION=9cb1dcda2182a8dca8483b230cda8b591a924c82

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include
endif

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
