include ../Makefile.drivers_common

FEATURES_OPTIONAL += periph_can

USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe

# The number of the CAN bus to use can be controlled via environment
# variable CAN_DEV:
CAN_DEV ?= 0
CFLAGS += -DCONFIG_CAN_DEV=$(CAN_DEV)

include $(RIOTBASE)/Makefile.include
