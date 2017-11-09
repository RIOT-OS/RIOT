ALIAS = linux phynode

# this basically implements a look-up table, is there a nicer way in make?
ifneq (,$(filter phynode,$(BOARD)))
  BOARD := pba-d-01-kw2x
endif
ifneq (,$(filter linux,$(BOARD)))
  BOARD := native
endif
