APPLICATION = driver_slip
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := msb-430 msb-430h nucleo-f334 stm32f0discovery weio

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_slip
USEMODULE += shell
USEMODULE += shell_commands

CFLAGS += -I$(CURDIR)

ifneq (,$(SLIP_UART))
  CFLAGS += -DSLIP_UART="UART_DEV($(SLIP_UART))"
else
  # set default
  CFLAGS += -DSLIP_UART="UART_DEV(UART_NUMOF-1)"
endif
ifneq (,$(SLIP_BAUDRATE))
  CFLAGS += -DSLIP_BAUDRATE=$(SLIP_BAUDRATE)
else
  # set default
  CFLAGS += -DSLIP_BAUDRATE=115200
endif

include $(RIOTBASE)/Makefile.include
