include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY :=    chronos \
                                msb-430 \
                                msb-430h \
                                nucleo-f031k6

DISABLE_MODULE += auto_init

ifneq (,$(filter nucleo32-f042,$(BOARD)))
  PROBLEM ?= 3
endif
ifneq (,$(filter nucleo-f030r8 nucleo32-l031 nucleo-l053r8 stm32f0discovery,$(BOARD)))
  PROBLEM ?= 5
endif
ifneq (,$(filter nucleo-f334r8,$(BOARD)))
  PROBLEM ?= 9
endif

PROBLEM ?= 12

CFLAGS += -DPROBLEM=$(PROBLEM)

TEST_ON_CI_WHITELIST += all

test:
	./tests/01-run.py

include $(RIOTBASE)/Makefile.include
