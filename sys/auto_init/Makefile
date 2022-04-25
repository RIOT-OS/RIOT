ifneq (,$(filter gnrc_netif_init,$(USEMODULE)))
  DIRS += netif
endif

ifneq (,$(filter saul_init,$(USEMODULE)))
  DIRS += saul
endif

ifneq (,$(filter auto_init_can,$(USEMODULE)))
  DIRS += can
endif

ifneq (,$(filter auto_init_loramac,$(USEMODULE)))
  DIRS += loramac
endif

ifneq (,$(filter auto_init_multimedia,$(USEMODULE)))
  DIRS += multimedia
endif

ifneq (,$(filter auto_init_usbus,$(USEMODULE)))
  DIRS += usb
endif

ifneq (,$(filter auto_init_security,$(USEMODULE)))
  DIRS += security
endif

ifneq (,$(filter auto_init_screen,$(USEMODULE)))
  DIRS += screen
endif

include $(RIOTBASE)/Makefile.base
