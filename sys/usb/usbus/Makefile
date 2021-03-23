SRCS := usbus.c
SRCS += usbus_hdrs.c

ifneq (,$(filter usbus_cdc_ecm,$(USEMODULE)))
    DIRS += cdc/ecm
endif
ifneq (,$(filter usbus_cdc_acm,$(USEMODULE)))
    DIRS += cdc/acm
endif
ifneq (,$(filter usbus_dfu,$(USEMODULE)))
    DIRS += dfu/
endif
ifneq (,$(filter usbus_hid,$(USEMODULE)))
    DIRS += hid
endif
include $(RIOTBASE)/Makefile.base
