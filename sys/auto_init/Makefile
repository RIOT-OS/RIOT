ifneq (,$(filter net_if,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include/
endif

ifneq (,$(filter nomac,$(USEMODULE)))
	INCLUDES += -I$(RIOTBASE)/sys/net/include/
endif

DIRS += $(AUTO_INIT_MODULES)

ifneq (,$(filter dev_ble_autoinit,$(USEMODULE)))
DIRS += $(RIOTBASE)/sys/auto_init/dev_ble
endif

include $(RIOTBASE)/Makefile.base
