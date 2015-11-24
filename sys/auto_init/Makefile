DIRS += $(AUTO_INIT_MODULES)

ifneq (,$(filter auto_init_gnrc_netif,$(USEMODULE)))
DIRS += netif
endif

ifneq (,$(filter auto_init_saul,$(USEMODULE)))
DIRS += saul
endif

include $(RIOTBASE)/Makefile.base
