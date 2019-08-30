.PHONY: info-objsize info-buildsizes info-build info-boards-supported \
        info-features-missing info-modules info-cpu \
        info-features-provided info-features-required \
        info-features-used \
        info-debug-variable-% info-toolchains-supported \
        check-toolchain-supported

info-objsize:
	@case "$(SORTROW)" in \
	  text) SORTROW=1 ;; \
	  data) SORTROW=2 ;; \
	  bss) SORTROW=3 ;; \
	  dec) SORTROW=4 ;; \
	  "") SORTROW=4 ;; \
	  *) echo "Usage: $(MAKE) info-objsize SORTROW=[text|data|bss|dec]" ; return ;; \
	esac; \
	printf '   text$(TAB)   data$(TAB)    bss$(TAB)    dec$(TAB)    hex$(TAB)filename$(NEWLINE)'; \
	$(SIZE) -d -B $(BASELIBS) | \
	  tail -n+2 | \
	  sed -e 's#$(BINDIR)##' | \
	  sort -rnk$${SORTROW}

info-buildsize:
	@$(SIZE) -d -B $(ELFFILE) || echo ''

info-build:
	$(info APPLICATION: $(APPLICATION))
	$(info APPDIR:      $(APPDIR))
	$(info )
	$(info supported boards:)
	$(info $($(MAKE) info-boards-supported))
	$(info )
	$(info BOARD:   $(BOARD))
	$(info CPU:     $(CPU))
	$(info MCU:     $(MCU))
	$(info )
	$(info RIOTBASE:  $(RIOTBASE))
	$(info RIOTBOARD: $(RIOTBOARD))
	$(info RIOTCPU:   $(RIOTCPU))
	$(info RIOTPKG:   $(RIOTPKG))
	$(info )
	$(info DEFAULT_MODULE: $(sort $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE))))
	$(info DISABLE_MODULE: $(sort $(DISABLE_MODULE)))
	$(info USEMODULE:      $(sort $(filter-out $(DEFAULT_MODULE), $(USEMODULE))))
	$(info )
	$(info ELFFILE: $(ELFFILE))
	$(info HEXFILE: $(HEXFILE))
	$(info BINFILE: $(BINFILE))
	$(info FLASHFILE: $(FLASHFILE))
	$(info )
	$(info FEATURES_USED:)
	$(info          $(or $(FEATURES_USED), -none-))
	$(info FEATURES_REQUIRED:)
	$(info          $(or $(sort $(FEATURES_REQUIRED)), -none-))
	$(info FEATURES_OPTIONAL_ONLY (optional that are not required, strictly "nice to have"):)
	$(info          $(or $(FEATURES_OPTIONAL_ONLY), -none-))
	$(info FEATURES_OPTIONAL_MISSING (missing optional features):)
	$(info          $(or $(FEATURES_OPTIONAL_MISSING), -none-))
	$(info FEATURES_PROVIDED (by the board or USEMODULE'd drivers):)
	$(info          $(or $(sort $(FEATURES_PROVIDED)), -none-))
	$(info FEATURES_MISSING (only non optional features):)
	$(info          $(or $(FEATURES_MISSING), -none-))
	$(info )
	$(info FEATURES_CONFLICT:     $(FEATURES_CONFLICT))
	$(info FEATURES_CONFLICT_MSG: $(FEATURES_CONFLICT_MSG))
	$(info FEATURES_CONFLICTING:)
	$(info          $(or $(FEATURES_CONFLICTING), -none-))
	$(info )
	$(info INCLUDES:$(patsubst %, $(NEWLINE)$(TAB)%, $(INCLUDES)))
	$(info )
	$(info CC:      $(CC))
	$(info CFLAGS:$(patsubst %, $(NEWLINE)$(TAB)%, $(CFLAGS)))
	$(info )
	$(info CXX:     $(CXX))
	$(info CXXUWFLAGS:$(patsubst %, $(NEWLINE)$(TAB)%, $(CXXUWFLAGS)))
	$(info CXXEXFLAGS:$(patsubst %, $(NEWLINE)$(TAB)%, $(CXXEXFLAGS)))
	$(info )
	$(info LINK:    $(LINK))
	$(info LINKFLAGS:$(patsubst %, $(NEWLINE)$(TAB)%, $(LINKFLAGS)))
	$(info )
	$(info OBJCOPY: $(OBJCOPY))
	$(info OFLAGS:  $(OFLAGS))
	$(info )
	$(info FLASHER: $(FLASHER))
	$(info FFLAGS:  $(FFLAGS))
	$(info )
	$(info TERMPROG:  $(TERMPROG))
	$(info TERMFLAGS: $(TERMFLAGS))
	$(info PORT:      $(PORT))
	$(info )
	$(info DEBUGGER:       $(DEBUGGER))
	$(info DEBUGGER_FLAGS: $(DEBUGGER_FLAGS))
	$(info )
	$(info DOWNLOAD_TO_FILE:   $(DOWNLOAD_TO_FILE))
	$(info DOWNLOAD_TO_STDOUT: $(DOWNLOAD_TO_STDOUT))
	$(info UNZIP_HERE:         $(UNZIP_HERE))
	$(info )
	$(info DEBUGSERVER:       $(DEBUGSERVER))
	$(info DEBUGSERVER_FLAGS: $(DEBUGSERVER_FLAGS))
	$(info )
	$(info RESET:       $(RESET))
	$(info RESET_FLAGS: $(RESET_FLAGS))
	$(info )
	$(info MAKEFILE_LIST:$(patsubst %, $(NEWLINE)$(TAB)%, $(abspath $(MAKEFILE_LIST))))
	@true

info-files: QUIET := 0
info-files:
	@( \
	  echo "$(abspath $(shell echo "$(MAKEFILE_LIST)"))" | tr ' ' '$(NEWLINE)'; \
	  CSRC="$$($(MAKE) USEPKG="" -Bn | grep -o -e "[^ ]\+\.[csS]$$" -e "[^ ]\+\.[csS][ \']" | grep -v -e "^\s*-D")"; \
	  echo "$$CSRC"; \
	  echo "$(RIOTBASE)/Makefile.base"; \
	  echo "$$CSRC" | xargs dirname -- | sort | uniq | xargs -I{} find {} -name "Makefile*"; \
	  echo "$$CSRC" | xargs $(CC) $(CFLAGS) $(INCLUDES) -MM 2> /dev/null | grep -o "[^ ]\+\.h"; \
	  if [ -n "$$SRCXX" ]; then \
	    CPPSRC="$$($(MAKE) -Bn USEPKG="" | grep -o -e "[^ ]\+\.cpp" | grep -v -e "^\s*-D")"; \
	    echo "$$CPPSRC"; \
	    echo "$$CPPSRC" | xargs dirname -- | sort | uniq | xargs -I{} find {} -name "Makefile*"; \
	    echo "$$CPPSRC" | xargs $(CXX) $(CXXFLAGS) $(INCLUDES) -MM 2> /dev/null | grep -o "[^ ]\+\.h"; \
	  fi; \
	  $(foreach pkg,$(USEPKG),find $(RIOTPKG)/$(pkg) -type f;) \
	) | sort | uniq | sed 's#$(RIOTBASE)/##'

info-modules:
	@for i in $(sort $(USEMODULE)); do echo $$i; done

info-cpu:
	@echo $(CPU)

info-features-provided:
	@for i in $(sort $(FEATURES_PROVIDED)); do echo $$i; done

info-features-required:
	@for i in $(sort $(FEATURES_REQUIRED)); do echo $$i; done

info-features-missing:
	@for i in $(FEATURES_MISSING); do echo $$i; done

info-features-used:
	@for i in $(FEATURES_USED); do echo $$i; done

info-debug-variable-%:
	$(info $($*))
	@true

info-toolchains-supported:
	$(info $(filter-out $(TOOLCHAINS_BLACKLIST),$(TOOLCHAINS_SUPPORTED)))
	@true

check-toolchain-supported:
	@exit $(if $(filter $(TOOLCHAIN),$(filter-out $(TOOLCHAINS_BLACKLIST),$(TOOLCHAINS_SUPPORTED))),0,1)
