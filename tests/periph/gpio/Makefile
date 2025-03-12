BOARD ?= samr34-xpro

include ../Makefile.periph_common

FEATURES_REQUIRED += periph_gpio
FEATURES_OPTIONAL += periph_gpio_irq
FEATURES_OPTIONAL += periph_gpio_fast_read
FEATURES_OPTIONAL += periph_gpio_tamper_wake

USEMODULE += shell_cmds_default
USEMODULE += benchmark

# disable native GPIOs for automatic test
ifneq (,$(filter native32 native64,$(BOARD)))
  USEMODULE += periph_gpio_mock
endif

BOARDS_BENCH_PORT_1 = \
    slstk3402a \
    z1 \
    #

# port 0 pins are used for serial output on these boards, e.g.: on slstk3402a
# PA5 (or port 0, pin 5) is used to control the BC enabling serial output,
# therefore test on port 1.
ifneq (,$(filter $(BOARD),$(BOARDS_BENCH_PORT_1)))
  PORT_UNDER_TEST ?= 1
else
  PORT_UNDER_TEST ?= 0
endif

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

# microbit qemu failing currently
TEST_ON_CI_BLACKLIST += microbit

include $(RIOTBASE)/Makefile.include

$(call target-export-variables,test,PORT_UNDER_TEST)

bench:
	tests/02-bench.py
