
MODULE_DEFS_NEEDED += \
  can_mbox \
  can_pm \
  can_trx \
  fdcan \
  #

ifneq (,$(filter can_isotp,$(USEMODULE)))
  DIRS += isotp
endif

ifneq (,$(filter conn_can,$(USEMODULE)))
  DIRS += conn
endif

include $(RIOTBASE)/Makefile.base
