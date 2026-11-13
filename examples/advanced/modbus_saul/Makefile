# name of your application
APPLICATION = modbus_saul

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Native target defaults to Modbus TCP transport, others will use
# Modbus RTU transport, unless overridden by the user:
ifneq (,$(filter native native32 native64,$(BOARD)))
  MODBUS_TRANSPORT ?= tcp
else
  MODBUS_TRANSPORT ?= rtu
endif

USEMODULE += modbus

ifeq ($(MODBUS_TRANSPORT),ascii)
  USEMODULE += modbus_ascii

  CFLAGS += -DMODBUS_TRANSPORT_ASCII
else ifeq ($(MODBUS_TRANSPORT),rtu)
  USEMODULE += modbus_rtu

  CFLAGS += -DMODBUS_TRANSPORT_RTU
else ifeq ($(MODBUS_TRANSPORT),tcp)
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  USEMODULE += modbus_tcp
  USEMODULE += netdev_default

  CFLAGS += -DMODBUS_TRANSPORT_TCP
else
  $(error MODBUS_TRANSPORT must be one of: ascii, rtu, tcp)
endif

# we want to use SAUL:
USEMODULE += saul_default
USEMODULE += shell_cmds_default

# use a mock GPIO driver on native, which has SAUL support:
ifneq (,$(filter native native32 native64,$(BOARD)))
  USEMODULE += periph_gpio_mock
endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
