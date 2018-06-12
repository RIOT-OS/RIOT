include ../Makefile.tests_common

DRIVER ?= mhz19_uart

USEMODULE += $(DRIVER)
USEMODULE += xtimer

ifneq (,$(filter mhz19_uart,$(USEMODULE)))
  # set default device parameters in case they are undefined
  TEST_UART ?= 1
  # export parameters
  CFLAGS += -DMHZ19_PARAM_UART_DEV=UART_DEV\($(TEST_UART)\)
endif

include $(RIOTBASE)/Makefile.include
