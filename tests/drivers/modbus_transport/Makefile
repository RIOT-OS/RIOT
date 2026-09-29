include ../Makefile.drivers_common

# Default test parameters
TEST_BAUDRATE ?= 9600
TEST_CLIENT_UART ?= 1
TEST_CLIENT_PIN_RTS ?= GPIO_UNDEF
TEST_SERVER_UART ?= 2
TEST_SERVER_PIN_RTS ?= GPIO_UNDEF

TEST_SERVER_PORT ?= 502
TEST_SERVER_ADDR ?= 192.168.100.11

# Native target defaults to Modbus TCP transport, others will use
# Modbus RTU transport, unless overridden by the user:
ifneq (,$(filter native native32 native64,$(BOARD)))
  MODBUS_TRANSPORT ?= tcp
else
  MODBUS_TRANSPORT ?= rtu
endif

USEMODULE += modbus
USEMODULE += shell
USEMODULE += tiny_strerror
USEMODULE += ztimer
USEMODULE += ztimer_msec
USEMODULE += ztimer_usec

ifeq ($(MODBUS_TRANSPORT),ascii)
  USEMODULE += modbus_ascii

  CFLAGS += -DMODBUS_TRANSPORT_ASCII
  CFLAGS += -DTEST_BAUDRATE=$(TEST_BAUDRATE)
  CFLAGS += -DTEST_CLIENT_UART=$(TEST_CLIENT_UART)
  CFLAGS += -DTEST_CLIENT_PIN_RTS=$(TEST_CLIENT_PIN_RTS)
  CFLAGS += -DTEST_SERVER_UART=$(TEST_SERVER_UART)
  CFLAGS += -DTEST_SERVER_PIN_RTS=$(TEST_SERVER_PIN_RTS)
else ifeq ($(MODBUS_TRANSPORT),rtu)
  USEMODULE += modbus_rtu

  CFLAGS += -DMODBUS_TRANSPORT_RTU
  CFLAGS += -DTEST_BAUDRATE=$(TEST_BAUDRATE)
  CFLAGS += -DTEST_CLIENT_UART=$(TEST_CLIENT_UART)
  CFLAGS += -DTEST_CLIENT_PIN_RTS=$(TEST_CLIENT_PIN_RTS)
  CFLAGS += -DTEST_SERVER_UART=$(TEST_SERVER_UART)
  CFLAGS += -DTEST_SERVER_PIN_RTS=$(TEST_SERVER_PIN_RTS)
else ifeq ($(MODBUS_TRANSPORT),tcp)
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  USEMODULE += modbus_tcp
  USEMODULE += netdev_default
  USEMODULE += shell_cmds_default

  CFLAGS += -DMODBUS_TRANSPORT_TCP
  CFLAGS += -DTEST_SERVER_PORT=$(TEST_SERVER_PORT)
  CFLAGS += -DTEST_SERVER_ADDR=\"$(TEST_SERVER_ADDR)\"
else
  $(error MODBUS_TRANSPORT must be one of: ascii, rtu, tcp)
endif

include $(RIOTBASE)/Makefile.include
