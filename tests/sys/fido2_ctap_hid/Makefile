BOARD ?= nrf52840dk
#BOARD ?= nrf52840dongle

include ../Makefile.sys_common

BOARD_WHITELIST = \
  nrf52840dk \
  nrf52840dongle \
  feather-nrf52840-sense

USEMODULE += fido2_ctap_transport_hid
USEPKG += fido2_tests

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

# Disable user presence tests
# Should be used when running fido2-test to make them run quicker
CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_UP=1

# Disable user LED animation
CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_LED=1

FEATURES_REQUIRED += periph_flashpage_aux
CFLAGS += -DCONFIG_FIDO2_MTD_DEV=mtd_aux
# Size of flash memory used to store persistent FIDO2-related data
SLOT_AUX_LEN := 0x1000

# FIDO2 tests except for the ones requiring user presence
#
# Use env -i because fido2-test has a depedency (pyscard) that needs to be
# compiled natively (x86-64). Therefore we need to clear the flags set by e.g.
# BOARD = nrf52840dk
fido2-test:
	env -i PATH=$(PATH) $(MAKE) -C $(PKGDIRBASE)/fido2_tests standard-tests

# FIDO2 user presence tests.
#
# Make sure to enable user presence tests by setting CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_UP=0.
# It is also recommended to set CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_LED=0 to have
# an indicator that signals when to press the button.
#
# Use env -i because fido2-test has a depedency (pyscard) that needs to be
# compiled natively (x86-64). Therefore we need to clear the flags set by e.g.
# BOARD = nrf52840dk
fido2-test-up:
	env -i PATH=$(PATH) $(MAKE) -C $(PKGDIRBASE)/fido2_tests up-tests

include $(RIOTBASE)/Makefile.include
