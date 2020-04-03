SRCS := usbus.c
SRCS += usbus_hdrs.c

ifneq (,$(filter usbus_cdc_ecm,$(USEMODULE)))
    DIRS += cdc/ecm
endif
ifneq (,$(filter usbus_cdc_acm,$(USEMODULE)))
    DIRS += cdc/acm
endif
include $(RIOTBASE)/Makefile.base
