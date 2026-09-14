RIOTMAKE ?= $(RIOTBASE)/makefiles

include $(RIOTMAKE)/utils/ansi.mk


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

# Colorizer functions:
#  These functions wrap a block of text in $(COLOR_X)...$(COLOR_RESET).
#  Do not nest calls to this functions or the colors will be wrong.
c_green  = $(COLOR_GREEN)$(1)$(COLOR_RESET)
c_red    = $(COLOR_RED)$(1)$(COLOR_RESET)
c_yellow = $(COLOR_YELLOW)$(1)$(COLOR_RESET)
c_purple = $(COLOR_PURPLE)$(1)$(COLOR_RESET)

# Functions to set the shell color one way
sh_green  = $(COLOR_ECHO) -n "$(COLOR_GREEN)" 1>&2
sh_red    = $(COLOR_ECHO) -n "$(COLOR_RED)" 1>&2
sh_yellow = $(COLOR_ECHO) -n "$(COLOR_YELLOW)" 1>&2
sh_purple = $(COLOR_ECHO) -n "$(COLOR_PURPLE)" 1>&2
sh_reset  = $(COLOR_ECHO) -n "$(COLOR_RESET)" 1>&2

# CI-safe echo functions (Recipe/Shell use):
#  These functions should be used as a substitute for `@$(COLOR_ECHO) ...` by
#  using `@$(call sh_echoinfo,("text"))`.
#
# Note: No space after the comma and mandatory parenthesis and quotation!
#
# Warning: You can not use `\n` or other control sequences in your text!
#  Apostrophes `'` and brackets `()` have to be escaped: `\'`, `\(`, `\)`.
#
# Background: The CI and other scripts interpret the output of Make printed to `stdout`.
#  To avoid confusing these scripts, user infos and red/yellow/green warnings
#  should be printed to `stderr` instead.
#
# Make interprets commata in the argument as separators, even when they are
# enclosed by quotation marks. The only workaround is to use extra parenthesis
# in the call and remove it afterwards with sed.
# See: https://vasvir.wordpress.com/2021/03/25/gnu-make-function-arguments-with-comma/
# We can't use the `c_green`, ... functions here, because the `sed` command would
# become very complex if it has to potentially ignore the escape sequences before
# and after the parenthesis it should remove.
sh_echoinfo    = $(COLOR_ECHO) "$(1)" | sed -e 's/^(\|)$$//g' 1>&2
sh_echogreen   = $(sh_green); $(sh_echoinfo); $(sh_reset)
sh_echored     = $(sh_red); $(sh_echoinfo); $(sh_reset)
sh_echoyellow  = $(sh_yellow); $(sh_echoinfo); $(sh_reset)
sh_echopurple  = $(sh_purple); $(sh_echoinfo); $(sh_reset)
sh_echoerr     = $(sh_echored)
sh_echowarn    = $(sh_echoyellow)

# CI-safe echo functions (Makefile use):
#  These functions should be used as a substitute for `$(info ...)` by using
#  `$(call echoinfo,("text"))`.
#
# Note: No space after the comma and mandatory parenthesis and quotation!
#
# Warning: You can not use `\n` or other control sequences in your text!
#  Apostrophes `'` and brackets `()` have to be escaped: `\'`, `\(`, `\)`.
#
# Background: The CI and other scripts interpret the output of Make printed to `stdout`.
#  To avoid confusing these scripts, user infos and red/yellow/green warnings
#  should be printed to `stderr` instead.
echoinfo    = $(shell $(call sh_echoinfo,$(1)))
echogreen   = $(shell $(call sh_echogreen,$(1)))
echored     = $(shell $(call sh_echored,$(1)))
echoyellow  = $(shell $(call sh_echoyellow,$(1)))
echopurple  = $(shell $(call sh_echopurple,$(1)))
echoerr     = $(shell $(call sh_echored,$(1)))
echowarn    = $(shell $(call sh_echoyellow,$(1)))
