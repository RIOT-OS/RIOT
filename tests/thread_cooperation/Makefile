include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano \
                             arduino-uno chronos i-nucleo-lrwan1 msb-430 \
                             msb-430h nucleo-f031k6 nucleo-f303k8 stm32l0538-disco

DISABLE_MODULE += auto_init

ifneq (,$(filter nucleo-f042k6,$(BOARD)))
  PROBLEM ?= 3
endif
ifneq (,$(filter nucleo-f030r8 nucleo-l031k6 nucleo-l053r8 stm32f0discovery stm32l0538-disco,$(BOARD)))
  PROBLEM ?= 5
endif
ifneq (,$(filter nucleo-f334r8,$(BOARD)))
  PROBLEM ?= 9
endif

PROBLEM ?= 12

CFLAGS += -DPROBLEM=$(PROBLEM)

include $(RIOTBASE)/Makefile.include
