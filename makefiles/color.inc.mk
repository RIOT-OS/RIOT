# Set colored output control sequences if the terminal supports it and if
# not disabled by the user

COLOR_GREEN  :=
COLOR_RED    :=
COLOR_YELLOW :=
COLOR_PURPLE :=
COLOR_RESET  :=
COLOR_ECHO   := /bin/echo

ifeq ($(CC_NOCOLOR),)
  IS_TERMINAL = $(if $(MAKE_TERMOUT),$(MAKE_TERMERR),)
  ifeq ($(IS_TERMINAL),)
    CC_NOCOLOR = 1
  else
    CC_NOCOLOR = 0
  endif
endif

ifeq ($(CC_NOCOLOR),0)
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
    COLOR_ECHO   := /bin/echo -e
  endif
endif
