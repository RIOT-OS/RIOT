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
	printf '   text\t   data\t    bss\t    dec\t    hex\tmodule\n'; \
	for i in $(sort $(BASELIBS:%.module=%)); \
	do \
	$(SIZE) -t -d -B $(BINDIR)/$$i/*.o 2> /dev/null | \
	  tail -n1 | \
	  sed -e "s#(TOTALS)#$$i#" | \
	  sed -e 's#$(BINDIR)##'; \
	done | sort -n -r -k $${SORTROW}

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
	@echo 'RIOTBASE:    $(RIOTBASE)'
	@echo 'BOARDDIR:    $(BOARDDIR)'
	@echo 'EXTERNAL_BOARD_DIRS:$(EXTERNAL_BOARD_DIRS)'
	@echo 'RIOTCPU:     $(RIOTCPU)'
	@echo 'RIOTPKG:     $(RIOTPKG)'
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
	@echo 'FEATURES_REQUIRED_ANY:'
	@echo '         $(or $(sort $(FEATURES_REQUIRED_ANY)), -none-)'
	@echo 'FEATURES_OPTIONAL_ONLY (optional that are not required, strictly "nice to have"):'
	@echo '         $(or $(FEATURES_OPTIONAL_ONLY), -none-)'
	@echo 'FEATURES_OPTIONAL_MISSING (missing optional features):'
	@echo '         $(or $(FEATURES_OPTIONAL_MISSING), -none-)'
	@echo 'FEATURES_PROVIDED (by the board or USEMODULE'"'"'d drivers):'
	@echo '         $(or $(sort $(FEATURES_PROVIDED)), -none-)'
	@echo 'FEATURES_MISSING (only non optional features):'
	@echo '         $(or $(FEATURES_MISSING), -none-)'
	@echo 'FEATURES_BLACKLIST (blacklisted features):'
	@echo '         $(or $(sort $(FEATURES_BLACKLIST)), -none-)'
	@echo 'FEATURES_USED_BLACKLISTED (used but blacklisted features):'
	@echo '         $(or $(sort $(FEATURES_USED_BLACKLISTED)), -none-)'
	@echo ''
	@echo 'FEATURES_CONFLICT:     $(FEATURES_CONFLICT)'
	@echo 'FEATURES_CONFLICT_MSG: $(FEATURES_CONFLICT_MSG)'
	@echo 'FEATURES_CONFLICTING:'
	@echo '         $(or $(FEATURES_CONFLICTING), -none-)'
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
	@echo 'PROG_DEV:  $(PROG_DEV)'
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

define json_string_or_null
$(or $(1:%="%"), null)
endef

# Convert a space separated list to a JSON array
define _to_json_string_list
[$(filter-out "","$(subst $(space),"$(comma)$(space)",$(1))")]
endef

# Strips out any existing quotes so that the generated JSON is valid, not necessary sensible
define to_json_string_list
$(call _to_json_string_list,$(strip $(subst ",,$(subst \",,$(1)))))
endef

# Crude json encoded build info.
# The output generated here is a best-effort JSON encoding, it is not perfect,
# converting the space separated lists in Make to a JSON array is flawed, it
# doesn't consider quoted parts as a single list item.  This mainly shows up in
# cflags such as:  -DNIMBLE_HOST_PRIO="(NIMBLE_CONTROLLER_PRIO + 1)", this is
# splitted into 3 array elements. To ensure that the generated JSON is valid,
# double quotes are currently stripped before generating the array.
info-build-json:
	@echo '{ '
	@echo '"APPLICATION": "$(APPLICATION)",'
	@echo '"APPDIR": "$(APPDIR)",'
	@echo '"BOARD": "$(BOARD)",'
	@echo '"CPU": "$(CPU)",'
	@echo '"MCU": "$(MCU)",'
	@echo '"RIOTBASE": "$(RIOTBASE)",'
	@echo '"BOARDDIR": "$(BOARDDIR)",'
	@echo '"RIOTCPU": "$(RIOTCPU)",'
	@echo '"RIOTPKG": "$(RIOTPKG)",'
	@echo '"EXTERNAL_BOARD_DIRS": $(call json_string_or_null $(EXTERNAL_BOARD_DIRS)),'
	@echo '"BINDIR": "$(BINDIR)",'
	@echo '"ELFFILE": "$(ELFFILE)",'
	@echo '"HEXFILE": "$(HEXFILE)",'
	@echo '"BINFILE": "$(BINFILE)",'
	@echo '"FLASHFILE": "$(FLASHFILE)",'
	@echo '"DEFAULT_MODULE": $(call to_json_string_list,$(sort $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE)))),'
	@echo '"DISABLE_MODULE": $(call to_json_string_list,$(sort $(DISABLE_MODULE))),'
	@echo '"USEMODULE": $(call to_json_string_list,$(sort $(filter-out $(DEFAULT_MODULE), $(USEMODULE)))),'
	@echo '"FEATURES_USED": $(call to_json_string_list,$(FEATURES_USED)),'
	@echo '"FEATURES_REQUIRED": $(call to_json_string_list,$(sort $(FEATURES_REQUIRED))),'
	@echo '"FEATURES_REQUIRED_ANY": $(call to_json_string_list,$(sort $(FEATURES_REQUIRED_ANY))),'
	@echo '"FEATURES_OPTIONAL_ONLY": $(call to_json_string_list,$(FEATURES_OPTIONAL_ONLY)),'
	@echo '"FEATURES_OPTIONAL_MISSING": $(call to_json_string_list,$(FEATURES_OPTIONAL_MISSING)),'
	@echo '"FEATURES_PROVIDED": $(call to_json_string_list,$(sort $(FEATURES_PROVIDED))),'
	@echo '"FEATURES_MISSING": $(call to_json_string_list,$(FEATURES_MISSING)),'
	@echo '"FEATURES_BLACKLIST": $(call to_json_string_list,$(sort $(FEATURES_BLACKLIST))),'
	@echo '"FEATURES_USED_BLACKLISTED": $(call to_json_string_list,$(sort $(FEATURES_USED_BLACKLISTED))),'
	@echo '"FEATURES_CONFLICT": $(call to_json_string_list,$(FEATURES_CONFLICT)),'
	@echo '"FEATURES_CONFLICTING": $(call to_json_string_list,$(FEATURES_CONFLICTING)),'
	@echo '"PREFIX": $(call json_string_or_null,$(PREFIX)),'
	@echo '"CC": "$(CC)",'
	@echo '"CXX": "$(CXX)",'
	@echo '"LINK": "$(LINK)",'
	@echo '"OBJCOPY": "$(OBJCOPY)",'
	@echo '"INCLUDES": $(call to_json_string_list,$(strip $(INCLUDES))),'
	@echo '"OFLAGS":  $(call to_json_string_list,$(OFLAGS)),'
	@echo '"CFLAGS": $(call to_json_string_list,$(CFLAGS)),'
	@echo '"CXXUWFLAGS": $(call to_json_string_list,$(CXXUWFLAGS)),'
	@echo '"CXXEXFLAGS": $(call to_json_string_list,$(CXXEXFLAGS)),'
	@echo '"LINKFLAGS": $(call to_json_string_list,$(LINKFLAGS))'
	@echo '}'


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
	@for i in $(FEATURES_MISSING); do echo $$i; done

info-features-used:
	@for i in $(FEATURES_USED); do echo $$i; done

info-debug-variable-%:
	@echo $($*)

info-toolchains-supported:
	@echo $(filter-out $(TOOLCHAINS_BLACKLIST),$(TOOLCHAINS_SUPPORTED))

check-toolchain-supported:
	@exit $(if $(filter $(TOOLCHAIN),$(filter-out $(TOOLCHAINS_BLACKLIST),$(TOOLCHAINS_SUPPORTED))),0,1)
