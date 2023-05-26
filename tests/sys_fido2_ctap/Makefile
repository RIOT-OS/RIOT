include ../Makefile.tests_common

# same as CTAP_STACKSIZE
CFLAGS += -DTHREAD_STACKSIZE_MAIN=15000

# Add unittest framework
USEMODULE += embunit

USEMODULE += fido2_ctap

# Disable user presence tests
CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_UP=1

# Disable user LED animation
CFLAGS += -DCONFIG_FIDO2_CTAP_DISABLE_LED=1

include $(RIOTBASE)/Makefile.include
