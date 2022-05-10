DEVELHELP = 0
BOARD ?= nrf52dk
include ../Makefile.tests_common

USEMODULE += app_metadata
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += stdio_nimble stdio_nimble_debug
USEMODULE += nimble_autoadv
CFLAGS += -DCONFIG_NIMBLE_AUTOADV_DEVICE_NAME='"tests/shell_ble"'

TESTRUNNER_SHELL_SKIP_REBOOT = 1
TESTRUNNER_RESET_BOARD_ON_STARTUP = 0

ifneq (,$(filter term,$(MAKECMDGOALS)))
  # Use a terminal that does not introduce extra characters into the stream.
  RIOT_TERMINAL ?= socat
else ifneq (,$(filter test,$(MAKECMDGOALS)))
  # Use the virtual serial port created by ble-serial
  RIOT_TERMINAL = picocom
endif

# The test requires some setup so it cannot currently be run
TEST_ON_CI_BLACKLIST += all

APP_SHELL_FMT ?= NONE

include $(RIOTBASE)/Makefile.include

# the test script skips tests if socat is not used
$(call target-export-variables,$(RIOT_TERMINAL),RIOT_TERMINAL)

# a reboot or a reset would disconnect the device from bluetooth and break the test
$(call target-export-variables,$(TESTRUNNER_SHELL_SKIP_REBOOT),TESTRUNNER_SHELL_SKIP_REBOOT)
$(call target-export-variables,$(TESTRUNNER_RESET_BOARD_ON_STARTUP),TESTRUNNER_RESET_BOARD_ON_STARTUP)
