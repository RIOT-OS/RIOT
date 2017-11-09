# list of known alias
ALIAS.phynode   = pba-d-01-kw2x
ALIAS.linux     = native

# this temporary variable is needed as we can not use BOARD when assigning BOARD
ALIAS_TARGET := $(ALIAS.$(BOARD))
ifneq (,($(ALIAS_TARGET)))
  override BOARD = $(ALIAS_TARGET)
endif
