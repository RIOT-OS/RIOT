# Add deprecated modules here
# Keep this list ALPHABETICALLY SORTED!!!!
ifeq ($(MAKELEVEL),0)

  ifneq (,$(filter $(DEPRECATED_BOARDS),$(BOARD)))
    $(shell $(COLOR_ECHO) "$(COLOR_RED)Deprecated board: $(COLOR_RESET)"\
                          "$(BOARD)" 1>&2)
  endif
endif
