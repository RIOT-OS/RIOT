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
  _COLOR_ECHO  = $(shell /bin/echo -e "$(1)")
  COLOR_GREEN  := $(call _COLOR_ECHO,\033[1;32m)
  COLOR_RED    := $(call _COLOR_ECHO,\033[1;31m)
  COLOR_YELLOW := $(call _COLOR_ECHO,\033[1;33m)
  COLOR_PURPLE := $(call _COLOR_ECHO,\033[1;35m)
  COLOR_RESET  := $(call _COLOR_ECHO,\033[0m)
endif

# Colorizer functions:
#  These functions wrap a block of text in $(COLOR_X)...$(COLOR_RESET).
#  Do not nest calls to this functions or the colors will be wrong.
c_green = $(COLOR_GREEN)$(1)$(COLOR_RESET)
c_red = $(COLOR_RED)$(1)$(COLOR_RESET)
c_yellow = $(COLOR_YELLOW)$(1)$(COLOR_RESET)
c_purple = $(COLOR_PURPLE)$(1)$(COLOR_RESET)

# Like Make's "warning" function, except the file and line number are not
# printed, and backlash escapes are interpreted.
warn = $(shell echo -e "$(1)" 1>&2)
