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
	@echo 'APPLICATION: $(APPLICATION)'
	@echo 'APPDIR:      $(APPDIR)'
	@echo ''
	@echo 'supported boards:'
	@echo $$($(MAKE) info-boards-supported)
	@echo ''
	@echo 'BOARD:   $(BOARD)'
	@echo 'CPU:     $(CPU)'
	@echo 'MCU:     $(MCU)'
	@echo ''
	@echo 'RIOTBASE:  $(RIOTBASE)'
	@echo 'RIOTBOARD: $(RIOTBOARD)'
	@echo 'RIOTCPU:   $(RIOTCPU)'
	@echo 'RIOTPKG:   $(RIOTPKG)'
	@echo ''
	@echo 'DEFAULT_MODULE: $(sort $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE)))'
	@echo 'DISABLE_MODULE: $(sort $(DISABLE_MODULE))'
	@echo 'USEMODULE:      $(sort $(filter-out $(DEFAULT_MODULE), $(USEMODULE)))'
	@echo ''
	@echo 'ELFFILE: $(ELFFILE)'
	@echo 'HEXFILE: $(HEXFILE)'
	@echo 'BINFILE: $(BINFILE)'
	@echo 'FLASHFILE: $(FLASHFILE)'
	@echo ''
	@echo 'FEATURES_USED:'
	@echo '         $(or $(FEATURES_USED), -none-)'
	@echo 'FEATURES_REQUIRED:'
	@echo '         $(or $(sort $(FEATURES_REQUIRED)), -none-)'
	@echo 'FEATURES_OPTIONAL_ONLY (optional that are not required, strictly "nice to have"):'
	@echo '         $(or $(FEATURES_OPTIONAL_ONLY), -none-)'
	@echo 'FEATURES_OPTIONAL_MISSING (missing optional features):'
	@echo '         $(or $(FEATURES_OPTIONAL_MISSING), -none-)'
	@echo 'FEATURES_PROVIDED (by the board or USEMODULE'"'"'d drivers):'
	@echo '         $(or $(sort $(FEATURES_PROVIDED)), -none-)'
	@echo 'FEATURES_MISSING (only non optional features):'
	@echo '         $(or $(FEATURES_MISSING), -none-)'
	@echo ''
	@echo 'FEATURES_CONFLICT:     $(FEATURES_CONFLICT)'
	@echo 'FEATURES_CONFLICT_MSG: $(FEATURES_CONFLICT_MSG)'
	@echo 'FEATURES_CONFLICTING:'
	@echo '         $(or $(FEATURES_CONFLICTING), -none-)'
	@echo ''
	@echo 'INCLUDES:$(patsubst %, $(NEWLINE)$(TAB)%, $(INCLUDES))'
	@echo ''
	@echo 'CC:      $(CC)'
	@echo 'CFLAGS:$(patsubst %, $(NEWLINE)$(TAB)%, $(CFLAGS))'
	@echo ''
	@echo 'CXX:     $(CXX)'
	@echo 'CXXUWFLAGS:$(patsubst %, $(NEWLINE)$(TAB)%, $(CXXUWFLAGS))'
	@echo 'CXXEXFLAGS:$(patsubst %, $(NEWLINE)$(TAB)%, $(CXXEXFLAGS))'
	@echo ''
	@echo 'LINK:    $(LINK)'
	@echo 'LINKFLAGS:$(patsubst %, $(NEWLINE)$(TAB)%, $(LINKFLAGS))'
	@echo ''
	@echo 'OBJCOPY: $(OBJCOPY)'
	@echo 'OFLAGS:  $(OFLAGS)'
	@echo ''
	@echo 'FLASHER: $(FLASHER)'
	@echo 'FFLAGS:  $(FFLAGS)'
	@echo ''
	@echo 'TERMPROG:  $(TERMPROG)'
	@echo 'TERMFLAGS: $(TERMFLAGS)'
	@echo 'PORT:      $(PORT)'
	@echo ''
	@echo 'DEBUGGER:       $(DEBUGGER)'
	@echo 'DEBUGGER_FLAGS: $(DEBUGGER_FLAGS)'
	@echo
	@echo 'DOWNLOAD_TO_FILE:   $(DOWNLOAD_TO_FILE)'
	@echo 'DOWNLOAD_TO_STDOUT: $(DOWNLOAD_TO_STDOUT)'
	@echo 'UNZIP_HERE:         $(UNZIP_HERE)'
	@echo ''
	@echo 'DEBUGSERVER:       $(DEBUGSERVER)'
	@echo 'DEBUGSERVER_FLAGS: $(DEBUGSERVER_FLAGS)'
	@echo ''
	@echo 'RESET:       $(RESET)'
	@echo 'RESET_FLAGS: $(RESET_FLAGS)'
	@echo ''
	@echo 'MAKEFILE_LIST:$(patsubst %, $(NEWLINE)$(TAB)%, $(abspath $(MAKEFILE_LIST)))'

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
	@echo $($*)

info-toolchains-supported:
	@echo $(filter-out $(TOOLCHAINS_BLACKLIST),$(TOOLCHAINS_SUPPORTED))

check-toolchain-supported:
	@exit $(if $(filter $(TOOLCHAIN),$(filter-out $(TOOLCHAINS_BLACKLIST),$(TOOLCHAINS_SUPPORTED))),0,1)
