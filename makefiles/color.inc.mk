# Set colored output control sequences if the terminal supports it and if
# not disabled by the user

COLOR_GREEN  :=
COLOR_RED    :=
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
  COLOR_GREEN  := \033[1;32m
  COLOR_RED    := \033[1;31m
  COLOR_YELLOW := \033[1;33m
  COLOR_PURPLE := \033[1;35m
  COLOR_RESET  := \033[0m
  ifeq ($(OS),Darwin)
    COLOR_ECHO   := echo -e
    SHELL=bash
  else
    COLOR_ECHO   := /bin/echo -e
  endif
endif
