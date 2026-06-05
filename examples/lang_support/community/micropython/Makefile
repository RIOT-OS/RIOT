# name of your application
APPLICATION = micropython

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# select MicroPython package
USEPKG += micropython

# include boot.py as header
BLOBS += boot.py

# MicroPython needs a larger stack
CFLAGS += '-DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_DEFAULT*4'

# use miniterm (instead of the default pyterm) in order to support control
# characters (CTRL-D ...)
RIOT_TERMINAL ?= miniterm

# enable modmachine support for peripherals if available
FEATURES_OPTIONAL += periph_adc
FEATURES_OPTIONAL += periph_spi

# For now `examples/%/tests" still rely on the test applicaton being reset after
# a terminal is opened to synchronize.
TESTRUNNER_RESET_AFTER_TERM ?= 1

# failing on native with floating point exception (#15870)
TEST_ON_CI_BLACKLIST += native32 native64

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

# Add target to run MicroPython's own test suite against the RIOT-OS port.
.PHONY: mpy-test
.NOTPARALLEL: mpy-test

# Add support for running MicroPython's own test suite. These tests assume a
# certain level of functionality, that is provided by this application.
mpy-test: $(TERMDEPS) pkg-prepare
	$(Q)$(COLOR_ECHO) "$(COLOR_YELLOW)Running MicroPython's own test suite." \
		"Expect a few tests to fail, especially with a heap size smaller than" \
		"16 KiB. Refer to 'pkg/micropython/doc.md' for more information." \
		"$(COLOR_RESET)"
	$(Q)cd $(RIOTBASE)/build/pkg/micropython/tests && \
		python3 ./run-tests.py --test-instance "$(PORT)"
