_alias = native
ifneq (,$(filter $(_alias),$(BOARD)))
  override BOARD := native32
  $(shell echo 'using BOARD="$(BOARD)" as "$(_alias)" on a 32-bit system' 1>&2)
endif

# aliases for other boards can be defined likewise in the future:
# _alias = old-board-name
# ifneq (,$(filter $(_alias),$(BOARD)))
#   override BOARD := new-board-name
#   $(shell echo 'warning: BOARD="$(_alias)" is a deprecated alias for "$(BOARD)". Start using BOARD="$(BOARD)" instead.' 1>&2)
# endif
