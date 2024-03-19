BOARD ?= samr21-xpro
include ../Makefile.periph_common

# Disable benchmark, optional features and longer in-app help message
# on boards with low RAM/ROM.
#
# In addition, all MSP430 based boards won't work with schedstatistics, as this
# imposes too much overhead for interrupt handlers (when unblocking the shell
# thread) for UART RX to work correctly.
LOW_MEMORY_BOARDS := \
    arduino-duemilanove \
    arduino-leonardo \
    arduino-nano \
    arduino-uno \
    atmega328p \
    atmega328p-xplained-mini \
    atmega8 \
    msb-430 \
    msb-430h \
    nucleo-l011k4 \
    olimex-msp430-h1611 \
    olimex-msp430-h2618 \
    samd10-xmini \
    stm32f030f4-demo \
    telosb \
    z1 \
    #

FEATURES_REQUIRED += periph_spi
FEATURES_OPTIONAL += periph_spi_on_qspi

ifeq (,$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
  FEATURES_OPTIONAL += periph_spi_reconfigure
  ENABLE_BENCHMARK ?= 1
  LOW_MEMORY := 0
else
  ENABLE_BENCHMARK ?= 0
  LOW_MEMORY := 1
endif

USEMODULE += ztimer_usec
USEMODULE += ztimer_sec
USEMODULE += shell_cmds_default

ifeq (1,$(ENABLE_BENCHMARK))
  USEMODULE += schedstatistics
endif

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

CFLAGS += -DENABLE_BENCHMARK=$(ENABLE_BENCHMARK)
CFLAGS += -DLOW_MEMORY=$(LOW_MEMORY)
