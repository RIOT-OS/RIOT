BOARD ?= native

include ../Makefile.sys_common

BOARD_WHITELIST = \
  native \
  nrf52840dk \
  nrf52840dongle

# same as CTAP_STACKSIZE
CFLAGS += -DTHREAD_STACKSIZE_MAIN=15000

# Add unittest framework
USEMODULE += embunit

USEMODULE += fido2_ctap

# Size of flash memory used to store persistent FIDO2-related data
SLOT_AUX_LEN := 0x1000

# Disable user presence tests
CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_UP=1

# Disable user LED animation
CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_LED=1

include $(RIOTBASE)/Makefile.include
