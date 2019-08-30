# Set colored output control sequences if the terminal supports it and if
# not disabled by the user

COLOR_GREEN  :=
COLOR_RED    :=
COLOR_YELLOW :=
COLOR_PURPLE :=
COLOR_RESET  :=
COLOR_ECHO   := /usr/bin/env echo

# Check if colored output is not disabled by user, i.e: CC_NOCOLOR unset
# or 0
ifneq ($(CC_NOCOLOR),1)
  IS_TERMINAL = $(if $(MAKE_TERMOUT),$(MAKE_TERMERR),)
  # Check if terminal support colored output
  ifneq ($(IS_TERMINAL),)
    _ANSI_ESC := $(shell printf "\033")
    COLOR_GREEN  := $(_ANSI_ESC)[1;32m
    COLOR_RED    := $(_ANSI_ESC)[1;31m
    COLOR_YELLOW := $(_ANSI_ESC)[1;33m
    COLOR_PURPLE := $(_ANSI_ESC)[1;35m
    COLOR_RESET  := $(_ANSI_ESC)[0m
    ifeq ($(OS),Darwin)
      COLOR_ECHO   := echo -e
      SHELL=bash
    else
      COLOR_ECHO   := /usr/bin/env echo -e
    endif
  endif
endif
