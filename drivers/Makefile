ifneq (,$(filter sht2x,$(USEMODULE)))
    DIRS += sht2x
endif
ifneq (,$(filter netdev_base,$(USEMODULE)))
    DIRS += netdev/base
endif
ifneq (,$(filter netdev_802154,$(USEMODULE)))
    DIRS += netdev/802154
endif
ifneq (,$(filter nrf24l01p,$(USEMODULE)))
    DIRS += nrf24l01p
endif

DIRS += $(dir $(wildcard $(addsuffix /Makefile, ${USEMODULE})))

include $(RIOTBASE)/Makefile.base
