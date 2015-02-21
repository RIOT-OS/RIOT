RELIC_URL=http://github.com/relic-toolkit/relic.git
RELIC_BRANCH=master

PKG_NAME=relic
PKG_URL=$(RELIC_URL)
PKG_VERSION=$(RELIC_BRANCH)
PKG_DIR=$(CURDIR)/$(PKG_NAME)

ifneq ($(RIOTBOARD),)
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
			-I$(RIOTBASE)/sys/posix/include -I$(RIOTBASE)/sys/posix/pnet/include
endif

.PHONY: all clean reset

all: $(PKG_DIR)/Makefile
	"$(MAKE)" -C $(PKG_DIR) && \
	cp $(PKG_DIR)/lib/librelic_s.a $(BINDIR)$(PKG_NAME).a

$(PKG_DIR)/comp-options.cmake: $(PKG_DIR)/.git/config
	cd "$(PKG_DIR)" && perl ../generate-cmake-xcompile.perl > comp-options.cmake

$(PKG_DIR)/Makefile: $(PKG_DIR)/comp-options.cmake
	cd "$(PKG_DIR)" && COMP="$(filter-out -Werror=old-style-definition -Werror=strict-prototypes, $(CFLAGS) ) " cmake -DCMAKE_TOOLCHAIN_FILE=comp-options.cmake -DCHECK=off -DTESTS=0 -DBENCH=0 -DSHLIB=off -Wno-dev $(RELIC_CONFIG_FLAGS) .
$(PKG_DIR)/.git/config:
	test -d "$(PKG_DIR)" || git clone "$(PKG_URL)" "$(PKG_DIR)"; \
		cd "$(PKG_DIR)" && git checkout -f "$(PKG_VERSION)"
	 cd "$(PKG_DIR)" && git am --ignore-whitespace $(CURDIR)/*.patch

clean::
	@echo "Cleaning up relic package..."
	rm -rf "$(PKG_DIR)"

distclean::
	rm -rf "$(PKG_DIR)"

Makefile.include:
	@true
