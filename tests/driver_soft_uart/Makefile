include ../Makefile.tests_common

USEMODULE += soft_uart
USEMODULE += soft_uart_modecfg

USEMODULE += shell

ifneq (,$(PIN_RX))
  CFLAGS += "-DSOFT_UART_PARAM_RX=$(PIN_RX)"
endif
ifneq (,$(PIN_TX))
  CFLAGS += "-DSOFT_UART_PARAM_TX=$(PIN_TX)"
endif

include $(RIOTBASE)/Makefile.include
