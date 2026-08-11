MODULE = usbus_cdc_acm

MODULE_DEFS_NEEDED += \
  usb_board_reset \
  #
SRC = cdc_acm.c

ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  SRC += cdc_acm_stdio.c
endif
include $(RIOTBASE)/Makefile.base
