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

ifneq (,$(filter skald_bthome_encrypt,$(USEMODULE)))
  $(shell $(COLOR_ECHO) "\n\n$(COLOR_RED)This currently does not work with Home Assistant!$(COLOR_RESET)\n\n" 1>&2)
endif

include $(RIOTBASE)/Makefile.base
