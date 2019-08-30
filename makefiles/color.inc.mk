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
    COLOR_GREEN  := $(ANSI_GREEN)
    COLOR_RED    := $(ANSI_RED)
    COLOR_YELLOW := $(ANSI_YELLOW)
    COLOR_PURPLE := $(ANSI_PURPLE)
    COLOR_RESET  := $(ANSI_RESET)
    ifeq ($(OS),Darwin)
      COLOR_ECHO   := echo -e
      SHELL=bash
    else
      COLOR_ECHO   := /usr/bin/env echo -e
    endif
  endif
endif
