include ../Makefile.tests_common

SLIP_BAUDRATE ?= 1000000

ifeq ($(BOARD), native)
  CFLAGS += -DNETDEV_TAP_MAX=2
  PORT ?= tap0 tap1
  USEMODULE += netdev_default
else
  USEMODULE += slipdev_l2addr
  CFLAGS += -DSLIPDEV_PARAMS={.uart=UART_DEV\(1\),.baudrate=$(SLIP_BAUDRATE)},{.uart=UART_DEV\(2\),.baudrate=$(SLIP_BAUDRATE)}
endif

# use GNRC by default
LWIP ?= 0

ifeq (0,$(LWIP))
  USEMODULE += auto_init_gnrc_netif
  USEMODULE += gnrc_icmpv6_echo
  USEMODULE += gnrc_ipv6_default
else
  USEMODULE += lwip_ipv6
  USEMODULE += lwip_ipv6_autoconfig
  USEMODULE += lwip_netdev
endif

USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += sock_udp

USEMODULE += ztimer_usec
USEMODULE += ztimer_msec

include $(RIOTBASE)/Makefile.include
