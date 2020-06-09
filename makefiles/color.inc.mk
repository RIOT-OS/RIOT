# Set colored output control sequences if the terminal supports it and if
# not disabled by the user

COLOR_GREEN  :=
COLOR_RED    :=
COLOR_PURPLE :=
COLOR_RESET  :=
COLOR_ECHO   := /usr/bin/env echo

# Check if colored output is not disabled by user, i.e: CC_NOCOLOR unset
# or 0
ifneq ($(CC_NOCOLOR),1)
  IS_TERMINAL = $(if $(MAKE_TERMOUT),$(MAKE_TERMERR),)
  # Check if terminal support colored output
  ifneq ($(IS_TERMINAL),)
    COLOR_GREEN  := \033[1;32m
    COLOR_RED    := \033[1;31m
    COLOR_YELLOW := \033[1;33m
    COLOR_PURPLE := \033[1;35m
    COLOR_RESET  := \033[0m
    ifeq ($(OS),Darwin)
      COLOR_ECHO   := echo -e
      SHELL=bash
    else
      COLOR_ECHO   := /usr/bin/env echo -e
    endif
  endif
endif
