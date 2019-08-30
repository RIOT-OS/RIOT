# Set colored output control sequences if the terminal supports it and if
# not disabled by the user

COLOR_GREEN  :=
COLOR_RED    :=
COLOR_YELLOW :=
COLOR_PURPLE :=
COLOR_RESET  :=

ifeq ($(CC_NOCOLOR),)
  IS_TERMINAL = $(if $(MAKE_TERMOUT),$(MAKE_TERMERR),)
  ifeq ($(IS_TERMINAL),)
    CC_NOCOLOR = 1
  else
    CC_NOCOLOR = 0
  endif
endif

ifeq ($(CC_NOCOLOR),0)
  COLOR_GREEN  := $(ANSI_GREEN)
  COLOR_RED    := $(ANSI_RED)
  COLOR_YELLOW := $(ANSI_YELLOW)
  COLOR_PURPLE := $(ANSI_PURPLE)
  COLOR_RESET  := $(ANSI_RESET)
endif
