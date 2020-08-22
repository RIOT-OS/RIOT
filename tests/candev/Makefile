include ../Makefile.tests_common
BOARD_WHITELIST := native

USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe

# define the CAN driver you want to use here
CAN_DRIVER ?= CAN_NATIVE

# prevent using native driver on non-native board
ifeq ($(CAN_DRIVER), CAN_NATIVE)
  ifneq ($(BOARD), native)
    $(error native can driver can only be used on native board!)
  endif
endif



ifeq ($(CAN_DRIVER), PERIPH_CAN)
# periph_can modules/variables go here

else ifeq ($(CAN_DRIVER), CAN_NATIVE)
# can_native modules/variables go here

endif

include $(RIOTBASE)/Makefile.include
