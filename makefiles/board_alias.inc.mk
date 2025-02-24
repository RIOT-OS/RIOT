# todo: use native64 on 64-bit systems (also update info below)
BOARD_ALIAS += native:native32

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
    $(shell echo 'using BOARD="$(_board)" as "$(_alias)" on a 32-bit system' 1>&2)
  else
    $(shell echo 'warning: BOARD="$(_alias)" is a deprecated alias. Consider using BOARD="$(_board)" instead.' 1>&2)
  endif
endif
