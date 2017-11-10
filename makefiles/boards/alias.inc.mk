# list of known alias
ALIAS.phynode   = pba-d-01-kw2x
#TODO remove this line (as its just for show)
ALIAS.linux     = native

# this temporary variable is needed as we can not use BOARD when assigning BOARD
ALIAS_TARGET := $(ALIAS.$(BOARD))
ifneq (,$(ALIAS_TARGET))
  ALIAS := $(BOARD)
  override BOARD := $(ALIAS_TARGET)
  MAKEOVERRIDES += BOARD=$(BOARD)
endif
