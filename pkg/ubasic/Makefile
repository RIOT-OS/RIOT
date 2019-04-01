PKG_NAME=ubasic
PKG_URL=https://github.com/adamdunkels/ubasic
PKG_VERSION=cc07193c231e21ecb418335aba5b199a08d4685c
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/ \
			  -f $(RIOTPKG)/ubasic/$(PKG_NAME).mk

include $(RIOTBASE)/pkg/pkg.mk
