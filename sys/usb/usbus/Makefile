SRCS := usbus.c
SRCS += usbus_hdrs.c

ifneq (,$(filter usbus_cdc_ecm,$(USEMODULE)))
    DIRS += cdc/ecm
endif
include $(RIOTBASE)/Makefile.base
