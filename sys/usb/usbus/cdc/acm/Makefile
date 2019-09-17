MODULE = usbus_cdc_acm
SRC = cdc_acm.c

ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  SRC += cdc_acm_stdio.c
endif
ifneq (,$(filter usbus_cdc_acm_uart,$(USEMODULE)))
  SRC += cdc_acm_uart.c
endif
include $(RIOTBASE)/Makefile.base
