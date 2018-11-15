# Set colored output control sequences if the terminal supports it and if
# not disabled by the user

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
else
  COLOR_ECHO   := /bin/echo
endif

# Colorizer functions:
#  These functions wrap a block of text in $(COLOR_X)...$(COLOR_RESET).
#  Do not nest calls to this functions or the colors will be wrong.
c_green = $(COLOR_GREEN)$(1)$(COLOR_RESET)
c_red = $(COLOR_RED)$(1)$(COLOR_RESET)
c_yellow = $(COLOR_YELLOW)$(1)$(COLOR_RESET)
c_purple = $(COLOR_PURPLE)$(1)$(COLOR_RESET)

# Output a color message to standard output.
color_info = $(shell $(COLOR_ECHO) $(1) 1>&2)
