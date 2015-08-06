ifneq (,$(filter nrf24l01p,$(USEMODULE)))
    DIRS += nrf24l01p
endif

DIRS += $(dir $(wildcard $(addsuffix /Makefile, ${USEMODULE})))

include $(RIOTBASE)/Makefile.base
