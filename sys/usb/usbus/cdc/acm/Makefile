MODULE = usbus_cdc_acm
SRC = cdc_acm.c

ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  SRC += cdc_acm_stdio.c
endif
include $(RIOTBASE)/Makefile.base
