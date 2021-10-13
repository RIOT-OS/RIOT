SRC = skald.c

ifneq (,$(filter skald_ibeacon,$(USEMODULE)))
  SRC += skald_ibeacon.c
endif

ifneq (,$(filter skald_eddystone,$(USEMODULE)))
  SRC += skald_eddystone.c
endif

include $(RIOTBASE)/Makefile.base
