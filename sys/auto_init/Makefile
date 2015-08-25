DIRS += $(AUTO_INIT_MODULES)

ifneq (,$(filter auto_init_gnrc_netif,$(USEMODULE)))
DIRS += netif
endif

ifneq (,$(filter dev_eth_autoinit,$(USEMODULE)))
DIRS += $(RIOTBASE)/sys/auto_init/dev_eth
endif

include $(RIOTBASE)/Makefile.base
