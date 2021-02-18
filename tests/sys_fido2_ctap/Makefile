BOARD ?= nrf52840dk
#BOARD ?= nrf52840dongle

include ../Makefile.tests_common

USEMODULE += fido2_ctap_hid
USEPKG += fido2_tests

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

# Disable user presence tests
# Should be used when running fido2-test to make them run quicker
#CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_UP=1
# Disable user LED animation
#CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_LED=1

# this will start the tests
fido2-test:
	$(MAKE) -C $(RIOTBASE)/build/pkg/fido2_tests run

include $(RIOTBASE)/Makefile.include
