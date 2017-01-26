PKG_NAME=relic
PKG_URL=https://github.com/relic-toolkit/relic.git
PKG_VERSION=cdcfaeef101d18c3231c3b46359c519dd72682e8
PKG_LICENSE=LGPL-2.1

.PHONY: all

all: $(PKG_BUILDDIR)/Makefile
	$(QQ) . $(RIOTBASE)/dist/tools/functions.sh ; \
		quiet "$(MAKE)" -C $(PKG_BUILDDIR)
	$(Q)cp $(PKG_BUILDDIR)/lib/librelic_s.a $(BINDIR)/$(PKG_NAME).a

$(PKG_BUILDDIR)/comp-options.cmake: fix_source
	$(Q)cd "$(PKG_BUILDDIR)" && perl $(PKG_DIR)/generate-cmake-xcompile.perl > comp-options.cmake

$(PKG_BUILDDIR)/Makefile: $(PKG_BUILDDIR)/comp-options.cmake
	$(Q) . $(RIOTBASE)/dist/tools/functions.sh ; \
		cd "$(PKG_BUILDDIR)" && \
		COMP="$(filter-out -Werror=old-style-definition -Werror=strict-prototypes, $(CFLAGS))" \
			quiet cmake \
				-DCMAKE_TOOLCHAIN_FILE=comp-options.cmake \
				-DCHECK=off -DTESTS=0 -DBENCH=0 -DSHLIB=off \
				-Wno-dev $(RELIC_CONFIG_FLAGS) .

CFLAGS += -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-function -Wno-newline-eof

fix_source: git-download
	$(Q)./fix-util_print_wo_args.sh $(PKG_BUILDDIR)
	$(Q)./fix-old-style-definitions.sh $(PKG_BUILDDIR)

clean::
	@rm -rf $(BINDIR)/$(PKG_NAME).a

include $(RIOTBASE)/pkg/pkg.mk
