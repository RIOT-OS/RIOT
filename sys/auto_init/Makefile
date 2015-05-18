ifneq (,$(filter net_if,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include/
endif

ifneq (,$(filter nomac,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include/
endif

DIRS += $(AUTO_INIT_MODULES)

ifneq (,$(filter auto_init_ng_netif,$(USEMODULE)))
DIRS += netif
endif

include $(RIOTBASE)/Makefile.base
