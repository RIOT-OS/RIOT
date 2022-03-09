# Add deprecated modules here
# Keep this list ALPHABETICALLY SORTED!!!!
ifeq ($(MAKELEVEL),0)
  DEPRECATED_CPUS += mips_pic32mx
  DEPRECATED_CPUS += mips_pic32mz
  ifneq (,$(filter $(DEPRECATED_CPUS),$(CPU)))
    $(shell $(COLOR_ECHO) "$(COLOR_RED)Deprecated cpu: $(COLOR_RESET)"\
                          "$(CPU)" 1>&2)
  endif
endif
