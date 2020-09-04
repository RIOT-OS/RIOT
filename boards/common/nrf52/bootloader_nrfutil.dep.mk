include $(RIOTBOARD)/common/makefiles/stdio_cdc_acm.mk

# enable bootloader reset over USB, requires USB bootloader to be used
ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  USEMODULE += usb_board_reset
endif
