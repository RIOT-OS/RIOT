# define board aliases as follows
# BOARD_ALIAS += alias-name:real-name
BOARD_ALIAS += feather-nrf52840:adafruit-feather-nrf52840-express
BOARD_ALIAS += feather-nrf52840-sense:adafruit-feather-nrf52840-sense

# default to native32 or native64 depending on host platform
_platform_bits := $(shell getconf LONG_BIT)
BOARD_ALIAS += native:native$(_platform_bits)

# if board is a known alias, have _BOARD_ALIAS_USED hold alias and board
_BOARD_ALIAS_USED := $(strip $(subst :, , $(filter $(BOARD):%, $(BOARD_ALIAS))))
ifneq (, $(_BOARD_ALIAS_USED))
  _alias := $(firstword $(_BOARD_ALIAS_USED))
  _board := $(lastword $(_BOARD_ALIAS_USED))
  # use real board name instead of alias from here on
  # we need to use override in case BOARD was specified on the command line
  override BOARD := $(_board)
  _whitelist := $(BOARD_WHITELIST)
  _blacklist := $(BOARD_BLACKLIST)
  _test_blacklist := $(TEST_ON_CI_BLACKLIST)
  BOARD_WHITELIST += $(if $(filter $(_alias),$(_whitelist)), $(_board))
  BOARD_BLACKLIST += $(if $(filter $(_alias),$(_blacklist)), $(_board))
  TEST_ON_CI_BLACKLIST += $(if $(filter $(_alias),$(_test_blacklist)), $(_board))
  # inform the user about the alias
  ifeq (native,$(_alias))
    $(shell echo 'using BOARD="$(_board)" as "$(_alias)" on a $(_platform_bits)-bit system' 1>&2)
  else
    $(shell echo 'warning: BOARD="$(_alias)" is a deprecated alias. Consider using BOARD="$(_board)" instead.' 1>&2)
  endif
endif
