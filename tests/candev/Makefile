include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe

FEATURES_OPTIONAL += periph_can

# define the CAN driver you want to use here
CAN_DRIVER ?= CAN_EXT

ifeq ($(CAN_DRIVER), CAN_EXT)
# external CAN driver modules/variables go here

endif

include $(RIOTBASE)/Makefile.include
