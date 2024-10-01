BOARD ?= nrf52840dk
#BOARD ?= nrf52840dongle

include ../Makefile.tests_common

USEMODULE += fido2_ctap_transport_hid
USEPKG += fido2_tests

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

# Disable user presence tests
# Should be used when running fido2-test to make them run quicker
CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_UP=1

# Disable user LED animation
CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_LED=1

# FIDO2 tests except for the ones requiring user presence
#
# Use env -i because fido2-test has a depedency (pyscard) that needs to be
# compiled natively (x86-64). Therefore we need to clear the flags set by e.g.
# BOARD = nrf52840dk
fido2-test:
	env -i PATH=$(PATH) $(MAKE) -C $(RIOTBASE)/build/pkg/fido2_tests

# FIDO2 user presence tests.
#
# Make sure to enable user presence tests by uncommenting CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_UP=1
#
# Use env -i because fido2-test has a depedency (pyscard) that needs to be
# compiled natively (x86-64). Therefore we need to clear the flags set by e.g.
# BOARD = nrf52840dk
fido2-test-up:
	env -i PATH=$(PATH) $(MAKE) -C $(RIOTBASE)/build/pkg/fido2_tests up-tests

include $(RIOTBASE)/Makefile.include
