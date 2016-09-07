PKG_NAME=yacoap
PKG_URL=git://github.com/riot-makers/yacoap.git
PKG_VERSION=a2c3fab92f62d092a7ca1a3201bc1eb0d2c7d2e3

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include
endif

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
