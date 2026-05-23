SRC = skald.c

ifneq (,$(filter skald_ibeacon,$(USEMODULE)))
  SRC += skald_ibeacon.c
endif

ifneq (,$(filter skald_eddystone,$(USEMODULE)))
  SRC += skald_eddystone.c
endif

ifneq (,$(filter skald_bthome,$(USEMODULE)))
  SRC += skald_bthome.c
endif

ifneq (,$(filter skald_bthome_saul,$(USEMODULE)))
  SRC += skald_bthome_saul.c
endif

include $(RIOTBASE)/Makefile.base
