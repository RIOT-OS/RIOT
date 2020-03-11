include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe

# define the CAN driver you want to use here
CAN_DRIVER ?= native

ifeq ($(CAN_DRIVER), PERIPH_CAN)
# periph_can modules/variables go here

else ifeq ($(CAN_DRIVER), CAN_NATIVE)
# can_native modules/variables go here

endif

CFLAGS += -DCAN_DRIVER_$(CAN_DRIVER)

include $(RIOTBASE)/Makefile.include
