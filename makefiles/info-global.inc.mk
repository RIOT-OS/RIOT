.PHONY: info-buildsizes info-buildsizes-diff info-features-missing \
        info-boards-features-missing

USEMODULE_GLOBAL := $(USEMODULE)
USEPKG_GLOBAL := $(USEPKG)
FEATURES_REQUIRED_GLOBAL := $(FEATURES_REQUIRED)
FEATURES_OPTIONAL_GLOBAL := $(FEATURES_OPTIONAL)
FEATURES_CONFLICT_GLOBAL := $(FEATURES_CONFLICT)
FEATURES_CONFLICT_MSG_GLOBAL := $(FEATURES_MSG_CONFLICT)
DISABLE_MODULE_GLOBAL := $(DISABLE_MODULE)
DEFAULT_MODULE_GLOBAL := $(DEFAULT_MODULE)

define board_missing_features
  BOARD             := $(1)
  USEMODULE         := $(USEMODULE_GLOBAL)
  USEPKG            := $(USEPKG_GLOBAL)
  DISABLE_MODULE    := $(DISABLE_MODULE_GLOBAL)
  DEFAULT_MODULE    := $(DEFAULT_MODULE_GLOBAL)
  FEATURES_REQUIRED := $(FEATURES_REQUIRED_GLOBAL)
  FEATURES_OPTIONAL := $(FEATURES_OPTIONAL_GLOBAL)
  FEATURES_CONFLICT := $(FEATURES_CONFLICT_GLOBAL)
  FEATURES_CONFLICT_MSG := $(FEATURES_CONFLICT_MSG_GLOBAL)

  # Remove board specific variables set by Makefile.features/Makefile.dep
  FEATURES_PROVIDED :=

  # Undefine variables that must not be defined when starting.
  # Some are sometime set as `?=`
  undefine CPU
  undefine CPU_MODEL

  include $(RIOTBASE)/Makefile.features

  include $(RIOTMAKE)/defaultmodules.inc.mk
  USEMODULE += $(filter-out $(DISABLE_MODULE), $(DEFAULT_MODULE))

  include $(RIOTBASE)/Makefile.dep

  ifneq (,$$(FEATURES_MISSING))
    BOARDS_FEATURES_MISSING += "$(1) $$(FEATURES_MISSING)"
    BOARDS_WITH_MISSING_FEATURES += $(1)
  endif
endef

BOARDS := $(filter $(if $(BOARD_WHITELIST), $(BOARD_WHITELIST), %), $(BOARDS))
BOARDS := $(filter-out $(BOARD_BLACKLIST), $(BOARDS))

BOARDS_WITH_MISSING_FEATURES :=
BOARDS_FEATURES_MISSING :=
$(foreach BOARD, $(BOARDS), $(eval $(call board_missing_features,$(BOARD))))
BOARDS := $(filter-out $(BOARDS_WITH_MISSING_FEATURES), $(BOARDS))

info-buildsizes: SHELL=bash
info-buildsizes:
	@echo "   text$(TAB)   data$(TAB)    bss$(TAB)    dec$(TAB)board"; \
	for board in $(BOARDS); do \
	    echo "$$(BOARD=$${board} $(MAKE) --no-print-directory info-buildsize 2>/dev/null | tail -n-1 | cut -f-4)" "$${board}"; \
	done;

info-buildsizes-diff: SHELL=bash
info-buildsizes-diff:
	@echo "text$(TAB)data$(TAB)bss$(TAB)dec$(TAB)BOARD/BINDIRBASE$(NEWLINE)"; \
	for board in $(BOARDS); do \
	  for BINDIRBASE in $${OLDBIN} $${NEWBIN}; do \
	      BINDIRBASE=$${BINDIRBASE} BOARD=$${board} $(MAKE) info-buildsize --no-print-directory 2>/dev/null | tail -n-1 | cut -f-4; \
	  done | \
	  while read -a OLD && read -a NEW; do \
	    for I in 0 1 2 3; do \
	      if [[ -n "$${NEW[I]}" && -n "$${OLD[I]}" ]]; then \
	        DIFF=$$(($${NEW[I]} - $${OLD[I]})); \
	        if [[ "$${DIFF}" -gt 0 ]]; then printf "$(COLOR_RED)"; fi; \
	        if [[ "$${DIFF}" -lt 0 ]]; then printf -n "$(COLOR_GREEN)"; fi; \
	      else \
	        DIFF="$(COLOR_RED)ERR"; \
	      fi; \
	      echo -ne "$${DIFF}$(TAB)$(COLOR_RESET)"; \
	    done; \
	    echo "$${board}"; \
	    for I in 0 1 2 3; do echo -n "$${OLD[I]-$(COLOR_RED)ERR$(COLOR_RESET)}$(TAB)"; done; echo "$${OLDBIN}"; \
	    for I in 0 1 2 3; do echo -n "$${NEW[I]-$(COLOR_RED)ERR$(COLOR_RESET)}$(TAB)"; done; echo "$${NEWBIN}$(NEWLINE)"; \
	  done; \
	done;

info-boards-supported:
	@echo $(BOARDS)

info-boards-features-missing:
	@for f in $(BOARDS_FEATURES_MISSING); do echo $${f}; done | column -t
