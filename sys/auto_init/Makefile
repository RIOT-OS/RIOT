ifneq (,$(filter auto_init_gnrc_netif,$(USEMODULE)))
  DIRS += netif
endif

ifneq (,$(filter auto_init_saul,$(USEMODULE)))
  DIRS += saul
endif

ifneq (,$(filter auto_init_storage,$(USEMODULE)))
  DIRS += storage
endif

ifneq (,$(filter auto_init_can,$(USEMODULE)))
  DIRS += can
endif

ifneq (,$(filter auto_init_gpio_exp,$(USEMODULE)))
  DIRS += gpio_exp
endif

include $(RIOTBASE)/Makefile.base
