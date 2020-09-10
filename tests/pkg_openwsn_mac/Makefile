BOARD ?= iotlab-m3

include ../Makefile.tests_common

# list of arm boards that provide at86rf2xx radios, can't require it so
# add whitelist
BOARD_WHITELIST = \
  fox \
  iotlab-m3 \
  iotlab-a8-m3 \
  samr21-xpro \
  samr30-xpro \
  #

# OpenWSN Modules
USEPKG += openwsn
USEMODULE += openwsn_mac
USEMODULE += openwsn_txtsnd

## Optional Module, usefull for debugginf
# USEMODULE += openwsn_serial

# Optional Modules
USEMODULE += openwsn_leds
USEMODULE += openwsn_debugpins

ifneq (,$(filter openwsn_serial,$(USEMODULE)))
  OPENSERIAL_BAUD ?=
  ifneq (,$(OPENSERIAL_BAUD))
    CFLAGS += -DOPENWSN_UART_BAUDRATE=$(OPENSERIAL_BAUD)
    ifneq (,$(filter stdio_null,$(USEMODULE)))
      BAUD ?= $(OPENSERIAL_BAUD)
    endif
  endif
endif

# To enable debugging prints on OpenWSN code
# export OPENWSN_LOG_LEVEL ?= LOG_ERROR

## Test application Modules
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands

CFLAGS += -fms-extensions

include $(RIOTBASE)/Makefile.include

# We want the highest possible frequency set for periph_rtt, but not all
# platforms can configure this value. use highest possible RTT_FREQUENCY
# for platforms that allow it
ifneq (,$(filter stm32,$(CPU)))
  RTT_FREQUENCY ?= RTT_MAX_FREQUENCY
  CFLAGS += -DRTT_FREQUENCY=$(RTT_FREQUENCY)
endif
