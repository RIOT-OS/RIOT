BOARD ?= native

include ../Makefile.sys_common

BOARD_WHITELIST = \
  native \
  nrf52840dk \
  nrf52840dongle \
  feather-nrf52840-sense

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

ifeq ($(BOARD), native)
  CFLAGS += -DCONFIG_FIDO2_MTD_DEV=MTD_0
else
  FEATURES_REQUIRED += periph_flashpage_aux
  CFLAGS += -DCONFIG_FIDO2_MTD_DEV=mtd_aux
  # Size of flash memory used to store persistent FIDO2-related data
  SLOT_AUX_LEN := 0x1000
endif

include $(RIOTBASE)/Makefile.include
