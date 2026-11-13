# code shared by the RP2040 and the RP2350
MODULE = cpu_common_rp2

# reset into the ROM bootloader via USB
ifneq (,$(filter usb_board_reset,$(USEMODULE)))
  SRC += usb_reset.c
endif

include $(RIOTMAKE)/periph.mk
