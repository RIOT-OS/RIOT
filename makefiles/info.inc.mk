.PHONY: info-objsize info-buildsizes info-build info-boards-supported \
        info-features-missing info-modules info-cpu \
        info-features-provided info-features-required \
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
	echo -e '   text\t   data\t    bss\t    dec\t    hex\tfilename'; \
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
	@echo ''
	@echo 'FEATURES_REQUIRED (excl. optional features):'
	@echo '         $(or $(sort $(filter-out $(FEATURES_OPTIONAL), $(FEATURES_REQUIRED))), -none-)'
	@echo 'FEATURES_OPTIONAL (strictly "nice to have"):'
	@echo '         $(or $(sort $(FEATURES_OPTIONAL)), -none-)'
	@echo 'FEATURES_PROVIDED (by the board or USEMODULE'"'"'d drivers):'
	@echo '         $(or $(sort $(FEATURES_PROVIDED)), -none-)'
	@echo 'FEATURES_MISSING (incl. optional features):'
	@echo '         $(or $(sort $(filter-out $(FEATURES_PROVIDED), $(FEATURES_REQUIRED))), -none-)'
	@echo 'FEATURES_MISSING (only non-optional features):'
	@echo '         $(or $(sort $(filter-out $(FEATURES_OPTIONAL) $(FEATURES_PROVIDED), $(FEATURES_REQUIRED))), -none-)'
	@echo ''
	@echo 'FEATURES_CONFLICT:     $(FEATURES_CONFLICT)'
	@echo 'FEATURES_CONFLICT_MSG: $(FEATURES_CONFLICT_MSG)'
	@echo ''
	@echo -e 'INCLUDES:$(patsubst %, \n\t%, $(INCLUDES))'
	@echo ''
	@echo 'CC:      $(CC)'
	@echo -e 'CFLAGS:$(patsubst %, \n\t%, $(CFLAGS))'
	@echo ''
	@echo 'CXX:     $(CXX)'
	@echo -e 'CXXUWFLAGS:$(patsubst %, \n\t%, $(CXXUWFLAGS))'
	@echo -e 'CXXEXFLAGS:$(patsubst %, \n\t%, $(CXXEXFLAGS))'
	@echo ''
	@echo 'LINK:    $(LINK)'
	@echo -e 'LINKFLAGS:$(patsubst %, \n\t%, $(LINKFLAGS))'
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
	@echo -e 'MAKEFILE_LIST:$(patsubst %, \n\t%, $(abspath $(MAKEFILE_LIST)))'

info-files: QUIET := 0
info-files:
	@( \
	  echo "$(abspath $(shell echo "$(MAKEFILE_LIST)"))" | tr ' ' '\n'; \
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
	@for i in $(sort $(filter-out $(FEATURES_PROVIDED), $(FEATURES_REQUIRED))); do echo $$i; done

info-debug-variable-%:
	@echo $($*)

info-toolchains-supported:
	@echo $(filter-out $(TOOLCHAINS_BLACKLIST),$(TOOLCHAINS_SUPPORTED))

check-toolchain-supported:
	@exit $(if $(filter $(TOOLCHAIN),$(filter-out $(TOOLCHAINS_BLACKLIST),$(TOOLCHAINS_SUPPORTED))),0,1)
