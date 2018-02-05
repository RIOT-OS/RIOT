# Set colored output control sequences if the terminal supports it and if
# not disabled by the user

COLOR_GREEN  :=
COLOR_RED    :=
COLOR_PURPLE :=
COLOR_RESET  :=
COLOR_ECHO   := /bin/echo

ifeq ($(CC_NOCOLOR),)
  available_colors:=$(shell tput colors 2> /dev/null)
  ifeq ($(available_colors),)
    CC_NOCOLOR = 1
  else ifeq ($(available_colors),-1)
    CC_NOCOLOR = 1
  else ifeq ($(available_colors),0)
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
