PKG_NAME=oonf_api
PKG_URL=https://github.com/OLSR/OONF.git
PKG_VERSION=v0.3.0
PKG_LICENSE=BSD-3-Clause

MODULE:=$(PKG_NAME)

# GCC 7.x fails on (intentional) fallthrough, thus disable implicit-fallthrough.
CFLAGS += -Wno-implicit-fallthrough

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)
	"$(MAKE)" $(BINDIR)/$(MODULE).a

$(BINDIR)/$(MODULE).a: $(BINDIR)/oonf_*.a
	mkdir -p $(BINDIR)/$(MODULE); cd $(BINDIR)/$(MODULE); for var in $?; do ar -x $$var; done; ar -r -c -s $(BINDIR)/$(MODULE).a *.o

include $(RIOTBASE)/pkg/pkg.mk
ifneq (,$(filter -Wformat-nonliteral -Wformat=2, $(CFLAGS)))
  CFLAGS += -Wno-format-nonliteral
endif
