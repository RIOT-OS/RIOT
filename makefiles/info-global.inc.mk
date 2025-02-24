.PHONY: info-buildsizes \
        info-buildsizes-diff \
        info-boards-supported \
        info-boards-features-missing \
        info-boards-features-blacklisted \
        info-boards-features-conflicting \
        #

BOARDDIR_GLOBAL := $(BOARDDIR)
USEMODULE_GLOBAL := $(USEMODULE)
USEPKG_GLOBAL := $(USEPKG)
FEATURES_REQUIRED_GLOBAL := $(FEATURES_REQUIRED)
FEATURES_REQUIRED_ANY_GLOBAL := $(FEATURES_REQUIRED_ANY)
FEATURES_OPTIONAL_GLOBAL := $(FEATURES_OPTIONAL)
FEATURES_CONFLICT_GLOBAL := $(FEATURES_CONFLICT)
FEATURES_CONFLICT_MSG_GLOBAL := $(FEATURES_MSG_CONFLICT)
DISABLE_MODULE_GLOBAL := $(DISABLE_MODULE)
DEFAULT_MODULE_GLOBAL := $(DEFAULT_MODULE)
FEATURES_BLACKLIST_GLOBAL := $(FEATURES_BLACKLIST)

define board_unsatisfied_features
  # BOARD might have been overwritten by `board_alias.ink.mk`
  # To be able to re-set it here, we need to use override again
  override BOARD    := $(1)
  USEMODULE         := $(USEMODULE_GLOBAL)
  USEPKG            := $(USEPKG_GLOBAL)
  DISABLE_MODULE    := $(DISABLE_MODULE_GLOBAL)
  DEFAULT_MODULE    := $(DEFAULT_MODULE_GLOBAL)
  FEATURES_REQUIRED := $(FEATURES_REQUIRED_GLOBAL)
  FEATURES_REQUIRED_ANY := $(FEATURES_REQUIRED_ANY_GLOBAL)
  FEATURES_OPTIONAL := $(FEATURES_OPTIONAL_GLOBAL)
  FEATURES_CONFLICT := $(FEATURES_CONFLICT_GLOBAL)
  FEATURES_CONFLICT_MSG := $(FEATURES_CONFLICT_MSG_GLOBAL)
  FEATURES_BLACKLIST:= $(FEATURES_BLACKLIST_GLOBAL)

  # Find matching board folder
  BOARDDIR := $$(word 1,$$(foreach dir,$$(BOARDSDIRS),$$(wildcard $$(dir)/$$(BOARD)/.)))

  # Remove board specific variables set by Makefile.features/Makefile.dep
  FEATURES_PROVIDED :=
  FEATURES_USED :=

  # Undefine variables that must not be defined when starting.
  # Some are sometime set as `?=`
  undefine CPU
  undefine CPU_MODEL
  undefine CPU_ARCH
  undefine CPU_CORE
  undefine CPU_FAM
  undefine RUST_TARGET
  undefine BOARD_VERSION

  include $(RIOTBASE)/Makefile.features
  # always select provided architecture features
  FEATURES_REQUIRED += $$(filter arch_%,$$(FEATURES_PROVIDED))
  # always select CPU core features
  FEATURES_REQUIRED += $$(filter cpu_core_%,$$(FEATURES_PROVIDED))
  # FEATURES_USED must be populated first in this case so that dependency
  # resolution can take optional features into account during the first pass.
  # Also: This allows us to skip resolution if already a missing feature is
  # detected
  include $(RIOTMAKE)/features_check.inc.mk
  ifneq (,$$(FEATURES_MISSING))
    # Skip full dependency resolution, as even without optional modules features
    # and architecture specific limitations already some features are missing
    BOARDS_FEATURES_MISSING += "$$(BOARD) $$(FEATURES_MISSING)"
    BOARDS_WITH_MISSING_FEATURES += $$(BOARD)
  else
    # add default modules
    include $(RIOTMAKE)/defaultmodules_regular.inc.mk
    USEMODULE += $$(filter-out $$(DISABLE_MODULE),$$(DEFAULT_MODULE))

    include $(RIOTMAKE)/dependency_resolution.inc.mk

    ifneq (,$$(FEATURES_MISSING))
      BOARDS_FEATURES_MISSING += "$$(BOARD) $$(FEATURES_MISSING)"
      BOARDS_WITH_MISSING_FEATURES += $$(BOARD)
    endif

    ifneq (,$$(FEATURES_USED_BLACKLISTED))
      BOARDS_FEATURES_USED_BLACKLISTED += "$$(BOARD) $$(FEATURES_USED_BLACKLISTED)"
      BOARDS_WITH_BLACKLISTED_FEATURES += $$(BOARD)
    endif

    ifneq (,$$(FEATURES_CONFLICTING))
      BOARDS_FEATURES_CONFLICTING += "$$(BOARD) $$(FEATURES_CONFLICTING)"
      BOARDS_WITH_CONFLICTING_FEATURES += $$(BOARD)
    endif
  endif

  ifneq (,$$(DEPENDENCY_DEBUG))
    $$(call file_save_dependencies_variables,dependencies_info-boards-supported)
  endif
endef

BOARDS := $(filter $(if $(BOARD_WHITELIST), $(BOARD_WHITELIST), %), $(BOARDS))
BOARDS := $(filter-out $(BOARD_BLACKLIST), $(BOARDS))

BOARDS_WITH_MISSING_FEATURES :=
BOARDS_FEATURES_MISSING :=
BOARDS_WITH_BLACKLISTED_FEATURES :=
BOARDS_FEATURES_USED_BLACKLISTED :=
BOARDS_FEATURES_CONFLICTING :=
BOARDS_WITH_CONFLICTING_FEATURES :=

$(foreach board,$(BOARDS),$(eval $(call board_unsatisfied_features,$(board))))
BOARDS := $(filter-out $(BOARDS_WITH_MISSING_FEATURES) \
                       $(BOARDS_WITH_BLACKLISTED_FEATURES) \
                       $(BOARDS_WITH_CONFLICTING_FEATURES), $(BOARDS))

info-buildsizes: SHELL=bash
info-buildsizes:
	@echo -e "   text\t   data\t    bss\t    dec\tboard"; \
	for board in $(BOARDS); do \
	    echo "$$(BOARD=$${board} $(MAKE) --no-print-directory info-buildsize 2>/dev/null | tail -n-1 | cut -f-4)" "$${board}"; \
	done;

info-buildsizes-diff: SHELL=bash
info-buildsizes-diff:
	@echo -e "text\tdata\tbss\tdec\tBOARD/BINDIRBASE\n"; \
	for board in $(BOARDS); do \
	  for BINDIRBASE in $${OLDBIN} $${NEWBIN}; do \
	      BINDIRBASE=$${BINDIRBASE} BOARD=$${board} $(MAKE) info-buildsize --no-print-directory 2>/dev/null | tail -n-1 | cut -f-4; \
	  done | \
	  while read -a OLD && read -a NEW; do \
	    for I in 0 1 2 3; do \
	      if [[ -n "$${NEW[I]}" && -n "$${OLD[I]}" ]]; then \
	        DIFF=$$(($${NEW[I]} - $${OLD[I]})); \
	        if [[ "$${DIFF}" -gt 0 ]]; then $(COLOR_ECHO) -n "$(COLOR_RED)"; fi; \
	        if [[ "$${DIFF}" -lt 0 ]]; then $(COLOR_ECHO) -n "$(COLOR_GREEN)"; fi; \
	      else \
	        DIFF="$(COLOR_RED)ERR"; \
	      fi; \
	      echo -ne "$${DIFF}\t$(COLOR_RESET)"; \
	    done; \
	    echo "$${board}"; \
	    for I in 0 1 2 3; do echo -ne "$${OLD[I]-$(COLOR_RED)ERR$(COLOR_RESET)}\t"; done; echo -e "$${OLDBIN}"; \
	    for I in 0 1 2 3; do echo -ne "$${NEW[I]-$(COLOR_RED)ERR$(COLOR_RESET)}\t"; done; echo -e "$${NEWBIN}\n"; \
	  done; \
	done;

info-boards-supported:
	@echo $(BOARDS)

info-boards-features-missing:
	@for f in $(BOARDS_FEATURES_MISSING); do echo $${f}; done | column -t

info-boards-features-blacklisted:
	@for f in $(BOARDS_FEATURES_USED_BLACKLISTED); do echo $${f}; done | column -t

info-boards-features-conflicting:
	@for f in $(BOARDS_FEATURES_CONFLICTING); do echo $${f}; done | column -t

generate-Makefile.ci:
	@$(RIOTTOOLS)/insufficient_memory/create_makefile.ci.sh


# Reset BOARDSDIR so unchanged for makefiles included after, for now only
# needed for buildtests.inc.mk
BOARDDIR := $(BOARDDIR_GLOBAL)
