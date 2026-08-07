# Configure nrf52dk as default board
BOARD ?= nrf52dk

# load the default test environment
include ../Makefile.pkg_common

# RIOT modules used in for this test
USEMODULE += xtimer
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += bluetil_ad

# Get the shared NimBLE and test configuration from the backend server
include $(RIOTBASE)/tests/pkg/nimble_l2cap_server/nimble.inc.mk

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DLOG_LEVEL=LOG_ALL
CFLAGS += -DCONFIG_ASSERT_VERBOSE=1

include $(RIOTBASE)/Makefile.include
