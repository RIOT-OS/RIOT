include ../Makefile.tests_common

# define the driver to be used for selected boards
ifneq (,$(filter stm32f429i-disc1 stm32f429i-disco stm32f3discovery,$(BOARD)))
  DRIVER := i3g4250d
endif

DRIVER ?= l3gd20h

USEMODULE += $(DRIVER)
USEMODULE += l3gxxxx
USEMODULE += ztimer
USEMODULE += ztimer_msec

include $(RIOTBASE)/Makefile.include
