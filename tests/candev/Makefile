include ../Makefile.tests_common

# the test currently only works with can_linux, so on "native"
BOARD_WHITELIST := native

USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe

# define the CAN driver you want to use here
CAN_DRIVER ?= CAN_NATIVE

ifeq ($(CAN_DRIVER), PERIPH_CAN)
# periph_can modules/variables go here

else ifeq ($(CAN_DRIVER), CAN_NATIVE)
# can_native modules/variables go here

endif

include $(RIOTBASE)/Makefile.include
