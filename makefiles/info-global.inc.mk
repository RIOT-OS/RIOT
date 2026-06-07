.PHONY: info-buildsizes-diff \
        info-boards-supported \
        info-boards-features-missing \
        info-boards-features-blacklisted \
        info-boards-features-conflicting \
        #

ifneq (,$(filter info-boards-collect, $(MAKECMDGOALS)))
  ifneq (1,$(INFO_OVERRIDE))
    $(error info-boards-collect should not be called directly!)
  endif
endif

BOARDS := $(filter $(if $(BOARDS_SUPPORTED), $(BOARDS_SUPPORTED), %), $(BOARDS))
BOARDS := $(filter-out $(BOARDS_UNSUPPORTED), $(BOARDS))

BOARD_CANDIDATES := $(addprefix .board-result-,$(BOARDS))

BOARDS_WITH_MISSING_FEATURES :=
BOARDS_FEATURES_MISSING :=
BOARDS_WITH_BLACKLISTED_FEATURES :=
BOARDS_FEATURES_USED_BLACKLISTED :=
BOARDS_FEATURES_CONFLICTING :=
BOARDS_WITH_CONFLICTING_FEATURES :=

ifneq (, $(filter info-boards-collect,$(MAKECMDGOALS)))
  # Find matching board folder
  BOARDDIR := $(word 1,$(foreach dir,$(BOARDSDIRS),$(wildcard $(dir)/$(BOARD)/.)))

  # Remove board specific variables set by Makefile.features/Makefile.dep
  FEATURES_PROVIDED :=
  FEATURES_USED :=

  include $(RIOTBASE)/Makefile.features
  # always select provided architecture features
  FEATURES_REQUIRED += $(filter arch_%,$(FEATURES_PROVIDED))
  # always select CPU core features
  FEATURES_REQUIRED += $(filter cpu_core_%,$(FEATURES_PROVIDED))
  # FEATURES_USED must be populated first in this case so that dependency
  # resolution can take optional features into account during the first pass.
  # Also: This allows us to skip resolution if already a missing feature is
  # detected
  include $(RIOTMAKE)/features_check.inc.mk
  ifneq (,$(FEATURES_MISSING))
    # Skip full dependency resolution, as even without optional modules features
    # and architecture specific limitations already some features are missing
    BOARDS_FEATURES_MISSING += "$(BOARD) $(FEATURES_MISSING)"
    BOARDS_WITH_MISSING_FEATURES += $(BOARD)
  else
    # add default modules
    include $(RIOTMAKE)/defaultmodules_regular.inc.mk
    USEMODULE += $(filter-out $(DISABLE_MODULE),$(DEFAULT_MODULE))

    include $(RIOTMAKE)/dependency_resolution.inc.mk

    ifneq (,$(FEATURES_MISSING))
      BOARDS_FEATURES_MISSING += "$(BOARD) $(FEATURES_MISSING)"
      BOARDS_WITH_MISSING_FEATURES += $(BOARD)
    endif

    ifneq (,$(FEATURES_USED_BLACKLISTED))
      BOARDS_FEATURES_USED_BLACKLISTED += "$(BOARD) $(FEATURES_USED_BLACKLISTED)"
      BOARDS_WITH_BLACKLISTED_FEATURES += $(BOARD)
    endif

    ifneq (,$(FEATURES_CONFLICTING))
      BOARDS_FEATURES_CONFLICTING += "$(BOARD) $(FEATURES_CONFLICTING)"
      BOARDS_WITH_CONFLICTING_FEATURES += $(BOARD)
    endif
  endif

  ifneq (,$(DEPENDENCY_DEBUG))
    $(call file_save_dependencies_variables,dependencies_info-boards-supported)
  endif

  BOARDS := $(filter-out $(BOARDS_WITH_MISSING_FEATURES) \
                         $(BOARDS_WITH_BLACKLISTED_FEATURES) \
                         $(BOARDS_WITH_CONFLICTING_FEATURES), $(BOARD))
endif

info-buildsizes-diff: SHELL=bash
info-buildsizes-diff:
	@echo -e "text\tdata\tbss\tdec\tBOARD/BINDIRBASE\n"; \
	COMMON_BOARDS=(); ONLY_OLD=(); ONLY_NEW=(); \
	SUPPORTED_BOARDS="$($(MAKE) info-boards-supported --no-print-directory -j)"; \
	for board in $(SUPPORTED_BOARDS); do \
	  if [[ -e "$${OLDBIN}/$${board}" && -e "$${NEWBIN}/$${board}" ]]; then \
	    COMMON_BOARDS+=($${board}); \
	  elif [[ -e "$${OLDBIN}/$${board}" ]]; then \
	    ONLY_OLD+=($${board}); \
	  elif [[ -e "$${NEWBIN}/$${board}" ]]; then \
	    ONLY_NEW+=($${board}); \
	  fi; \
	done; \
	for board in "$${COMMON_BOARDS[@]}"; do \
	  for BINDIRBASE in $${OLDBIN} $${NEWBIN}; do \
	      env --unset=MAKEFLAGS BINDIRBASE=$${BINDIRBASE} BOARD=$${board} $(MAKE) info-buildsize --no-print-directory 2>/dev/null | tail -n-1 | cut -f-4; \
	  done | \
	  while read -a OLD && read -a NEW; do \
	    for I in 0 1 2 3; do \
	      if [[ -n "$${NEW[I]}" && -n "$${OLD[I]}" ]]; then \
	        DIFF=$$(($${NEW[I]} - $${OLD[I]})); \
	        if [[ "$${DIFF}" -gt 0 ]]; then $(COLOR_ECHO) -n "$(COLOR_RED)"; fi; \
	        if [[ "$${DIFF}" -eq 0 ]]; then $(COLOR_ECHO) -n "$(COLOR_YELLOW)"; fi; \
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
	done; \
	if [[ "$${#ONLY_OLD[@]}" -gt 0 ]]; then \
	  $(COLOR_ECHO) "$(COLOR_YELLOW)Boards in OLDBIN without complementary NEWBIN: $${ONLY_OLD[*]}$(COLOR_RESET)"; \
	fi; \
	if [[ "$${#ONLY_NEW[@]}" -gt 0 ]]; then \
	  $(COLOR_ECHO) "$(COLOR_YELLOW)Boards in NEWBIN without complementary OLDBIN: $${ONLY_NEW[*]}$(COLOR_RESET)"; \
	fi;


# Temporary files that contain the information gathered by the submake
# processes started for each individual board. These reside in the `bin/`
# subdirectory so they are removed by `make (dist)clean` and ignored by git.
SUPPORT_FILES := bin/.INFO_BOARDS_SUPPORTED
SUPPORT_FILES += bin/.INFO_BOARDS_FEATURES_MISSING
SUPPORT_FILES += bin/.INFO_BOARDS_FEATURES_BLACKLISTED
SUPPORT_FILES += bin/.INFO_BOARDS_FEATURES_CONFLICTING

.PHONY: $(SUPPORT_FILES)
$(SUPPORT_FILES):
	@rm -rf $@
	@mkdir -p bin/
	@touch $@

.board-result-%:
	@$(MAKE) BOARD=$* INFO_OVERRIDE=1 info-boards-collect --no-print-directory

info-boards-supported: bin/.INFO_BOARDS_SUPPORTED $(BOARD_CANDIDATES)
	@cat bin/.INFO_BOARDS_SUPPORTED |  sort | xargs echo

info-boards-features-missing: bin/.INFO_BOARDS_FEATURES_MISSING $(BOARD_CANDIDATES)
	@cat bin/.INFO_BOARDS_FEATURES_MISSING | column -t

info-boards-features-blacklisted: bin/.INFO_BOARDS_FEATURES_BLACKLISTED $(BOARD_CANDIDATES)
	@cat bin/.INFO_BOARDS_FEATURES_BLACKLISTED | column -t

info-boards-features-conflicting: bin/.INFO_BOARDS_FEATURES_CONFLICTING $(BOARD_CANDIDATES)
	@cat bin/.INFO_BOARDS_FEATURES_CONFLICTING | column -t

generate-Makefile.ci:
	@$(RIOTTOOLS)/insufficient_memory/create_makefile.ci.sh


# This target is run as a submake command by the `.board-result-%` for each
# board in a new environment, since the evaluation of the features is only done
# for one board at a time and the results are saved in temporary files.
# These files are then evaluated by the `info-boards-*` targets.
PHONY: info-boards-collect
info-boards-collect:
	@if [ ! -z '$(BOARDS)' ]; then \
	  echo '$(BOARDS) ' >> bin/.INFO_BOARDS_SUPPORTED; \
	fi
	@if [ ! -z '$(BOARDS_FEATURES_MISSING)' ]; then \
	  echo $(BOARDS_FEATURES_MISSING) >> bin/.INFO_BOARDS_FEATURES_MISSING; \
	fi;
	@if [ ! -z '$(BOARDS_FEATURES_USED_BLACKLISTED)' ]; then \
	  echo $(BOARDS_FEATURES_USED_BLACKLISTED) >> bin/.INFO_BOARDS_FEATURES_BLACKLISTED; \
	fi;
	@if [ ! -z '$(BOARDS_FEATURES_CONFLICTING)' ]; then \
	  echo $(BOARDS_FEATURES_CONFLICTING) >> bin/.INFO_BOARDS_FEATURES_CONFLICTING; \
	fi;
