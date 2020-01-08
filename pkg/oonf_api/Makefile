PKG_NAME=oonf_api
PKG_URL=https://github.com/OLSR/OONF.git
PKG_VERSION=v0.3.0
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

MODULE:=$(PKG_NAME)

# GCC 7.x fails on (intentional) fallthrough, thus disable implicit-fallthrough.
CFLAGS += -Wno-implicit-fallthrough

COMBINED_ARCHIVE = $(BINDIR)/$(MODULE).a

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)
	"$(MAKE)" $(COMBINED_ARCHIVE)

PARTIAL_ARCHIVES = $(wildcard $(BINDIR)/oonf_*.a)

$(COMBINED_ARCHIVE): $(BINDIR)/$(MODULE).mri $(PARTIAL_ARCHIVES)
	ar -M < $<

define ADDLIB_TEMPLATE
addlib $1

endef

define MRI_TEMPLATE
create $1
$(foreach a,$2,$(call ADDLIB_TEMPLATE,$a))
save
end
endef

$(BINDIR)/$(MODULE).mri:
	$(file >$@,$(call MRI_TEMPLATE,$(COMBINED_ARCHIVE),$(PARTIAL_ARCHIVES)))
	@true

ifneq (,$(filter -Wformat-nonliteral -Wformat=2, $(CFLAGS)))
  CFLAGS += -Wno-format-nonliteral
endif
