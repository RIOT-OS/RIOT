include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY :=    chronos \
                                msb-430 \
                                msb-430h \
                                nucleo32-f031

DISABLE_MODULE += auto_init

ifneq (,$(filter nucleo32-f042,$(BOARD)))
  PROBLEM ?= 3
endif
ifneq (,$(filter nucleo-f030 nucleo32-l031 nucleo-l053 stm32f0discovery,$(BOARD)))
  PROBLEM ?= 5
endif
ifneq (,$(filter nucleo-f334,$(BOARD)))
  PROBLEM ?= 9
endif

PROBLEM ?= 12

CFLAGS += -DPROBLEM=$(PROBLEM)

test:
	./tests/01-run.py

include $(RIOTBASE)/Makefile.include
