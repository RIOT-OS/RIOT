include ../Makefile.tests_common

ifneq (,$(filter nucleo-f042k6,$(BOARD)))
  PROBLEM ?= 3
endif
ifneq (,$(filter nucleo-f030r8 nucleo-l031k6 nucleo-l053r8 stm32f0discovery stm32l0538-disco,$(BOARD)))
  PROBLEM ?= 5
endif
ifneq (,$(filter nucleo-f303k8,$(BOARD)))
  PROBLEM ?= 7
endif
ifneq (,$(filter nucleo-f334r8,$(BOARD)))
  PROBLEM ?= 9
endif

PROBLEM ?= 12

CFLAGS += -DPROBLEM=$(PROBLEM)

include $(RIOTBASE)/Makefile.include
