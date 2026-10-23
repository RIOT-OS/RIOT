# peripheral drivers shared by the RP2040 and the RP2350
MODULE = rpx0xx_periph_common

# reset into the ROM bootloader via USB
ifneq (,$(filter usb_board_reset,$(USEMODULE)))
  SRC += usb_reset.c
endif

include $(RIOTMAKE)/periph.mk
