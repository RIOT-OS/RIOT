ifneq (,$(filter netdev_base,$(USEMODULE)))
    DIRS += netdev/base
endif
ifneq (,$(filter netdev_802154,$(USEMODULE)))
    DIRS += netdev/802154
endif

DIRS += $(dir $(wildcard $(addsuffix /Makefile, ${USEMODULE})))

include $(RIOTBASE)/Makefile.base
