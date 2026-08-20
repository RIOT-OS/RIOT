
MODULE_DEFS_NEEDED += \
  l3gxxxx_config \
  l3gxxxx_fifo \
  l3gxxxx_irq \
  l3gxxxx_irq_event \
  #
include ../Makefile.drivers_common

# define the driver to be used for selected boards
ifneq (,$(filter stm32f429i-disc1 stm32f429i-disco stm32f3discovery,$(BOARD)))
  DRIVER := i3g4250d
endif

ifneq (,$(filter iotlab-m3 iotlab-a8-m3,$(BOARD)))
  DRIVER := l3g4200d_ng
endif

DRIVER ?= l3gd20h

USEMODULE += $(DRIVER)
USEMODULE += l3gxxxx
USEMODULE += ztimer
USEMODULE += ztimer_msec

include $(RIOTBASE)/Makefile.include
