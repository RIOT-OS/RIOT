PKG_NAME=relic
PKG_URL=https://github.com/relic-toolkit/relic.git
PKG_VERSION=cdcfaeef101d18c3231c3b46359c519dd72682e8
PKG_LICENSE=LGPL-2.1

.PHONY: all

CFLAGS += -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-function -Wno-newline-eof

TOOLCHAIN_FILE=$(PKG_BUILDDIR)/xcompile-toolchain.cmake

all: $(PKG_BUILDDIR)/Makefile
	$(MAKE) -C $(PKG_BUILDDIR) && \
	cp $(PKG_BUILDDIR)/lib/librelic_s.a $(BINDIR)/$(PKG_NAME).a

$(PKG_BUILDDIR)/Makefile: $(TOOLCHAIN_FILE)
	cd $(PKG_BUILDDIR) && \
	COMP="$(filter-out -Werror=old-style-definition -Werror=strict-prototypes, $(CFLAGS) ) " \
	cmake -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		  -DCHECK=off -DTESTS=0 -DBENCH=0 -DSHLIB=off -Wno-dev $(RELIC_CONFIG_FLAGS) .

$(TOOLCHAIN_FILE): fix_source
	$(RIOTBASE)/dist/tools/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

fix_source: git-download
	./fix-util_print_wo_args.sh $(PKG_BUILDDIR)
	./fix-old-style-definitions.sh $(PKG_BUILDDIR)

clean::
	@rm -rf $(BINDIR)/$(PKG_NAME).a

include $(RIOTBASE)/pkg/pkg.mk
