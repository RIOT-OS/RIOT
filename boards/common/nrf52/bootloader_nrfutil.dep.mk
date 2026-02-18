include $(RIOTBOARD)/common/makefiles/stdio_cdc_acm.dep.mk

# enable bootloader reset over USB, requires CDC ACM to be used
ifneq (,$(filter stdio_cdc_acm stdio_tinyusb_cdc_acm,$(USEMODULE)))
  USEMODULE += usb_board_reset
endif
