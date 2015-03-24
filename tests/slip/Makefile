APPLICATION = driver_slip
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := stm32f0discovery

USEMODULE += ng_netbase
USEMODULE += ng_pktdump
USEMODULE += ng_slip
USEMODULE += shell
USEMODULE += shell_commands

ifneq (,$(SLIP_UART))
  CFLAGS += -DSLIP_UART=$(SLIP_UART)
else
  # set default
  CFLAGS += -DSLIP_UART=UART_0
endif
ifneq (,$(SLIP_BAUDRATE))
  CFLAGS += -DSLIP_BAUDRATE=$(SLIP_BAUDRATE)
else
  # set default
  CFLAGS += -DSLIP_BAUDRATE=115200
endif

include $(RIOTBASE)/Makefile.include
