MODULE = tinyusb_hw

ifneq (,$(filter saml21 samd5x samd21,$(CPU)))
  SRC = hw_sam0.c
else ifeq (stm32,$(CPU))
  ifeq (,$(filter f1 f3 wb,$(CPU_FAM)))
    # All STM32 families except for F1, F3 and WB use the Synopsys DWC2 USB OTG IP core
    SRC += hw_stm32_otg.c
  else ifneq (,$(filter stm32f105% stm32f107%,$(CPU_MODEL)))
    # STM32F105xx and STM32F107xx also use the Synopsys DWC2 USB OTG IP core
    SRC += hw_stm32_otg.c
  else
    # All other STM32 MCUs use the USB Device FS IP core.
    SRC += hw_stm32_fsdev.c
  endif
else
  SRC = hw_$(CPU).c
endif

include $(RIOTBASE)/Makefile.base
