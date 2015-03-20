ifneq (,$(filter netdev_base,$(USEMODULE)))
    DIRS += netdev/base
endif
ifneq (,$(filter netdev_802154,$(USEMODULE)))
    DIRS += netdev/802154
endif
ifneq (,$(filter nrf24l01p,$(USEMODULE)))
    DIRS += nrf24l01p
endif
ifneq (,$(filter ng_at86rf2xx,$(USEMODULE)))
    DIRS += ng_at86rf2xx
endif

DIRS += $(dir $(wildcard $(addsuffix /Makefile, ${USEMODULE})))

include $(RIOTBASE)/Makefile.base
