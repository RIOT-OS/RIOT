PKG_NAME=relic
PKG_URL=http://github.com/relic-toolkit/relic.git
PKG_VERSION=cdcfaeef101d18c3231c3b46359c519dd72682e8

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include -I$(RIOTBASE)/sys/posix/pnet/include
endif

.PHONY: all

all: $(PKG_BUILDDIR)/Makefile
	@echo "Building relic package..." ; \
		OUTPUT="$$("$(MAKE)" -C $(PKG_BUILDDIR) 2>&1)" || \
		{ echo $$OUTPUT; false; } && \
		cp $(PKG_BUILDDIR)/lib/librelic_s.a $(BINDIR)$(PKG_NAME).a

$(PKG_BUILDDIR)/comp-options.cmake: fix_source
	cd "$(PKG_BUILDDIR)" && perl $(PKG_DIR)/generate-cmake-xcompile.perl > comp-options.cmake

$(PKG_BUILDDIR)/Makefile: $(PKG_BUILDDIR)/comp-options.cmake
	@echo "Configuring relic package..."
	@cd "$(PKG_BUILDDIR)" && COMP="$(filter-out -Werror=old-style-definition -Werror=strict-prototypes, $(CFLAGS) ) " cmake -DCMAKE_TOOLCHAIN_FILE=comp-options.cmake -DCHECK=off -DTESTS=0 -DBENCH=0 -DSHLIB=off -Wno-dev $(RELIC_CONFIG_FLAGS) . \
		> $(PKG_BUILDDIR)/relic_config.out 2>&1 || { cat $(PKG_BUILDDIR)/relic_config.out; false; }

fix_source: git-download
	./fix-util_print_wo_args.sh $(PKG_BUILDDIR)
	./fix-old-style-definitions.sh $(PKG_BUILDDIR)

clean::
	@rm -rf $(BINDIR)$(PKG_NAME).a
include $(RIOTBASE)/pkg/pkg.mk
