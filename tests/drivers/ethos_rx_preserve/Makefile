BOARD ?= native64

include ../Makefile.drivers_common

USEMODULE += ethos
USEMODULE += test_utils_netdev_eth_minimal

INCLUDES += -I$(APPDIR)

TEST_DEPS += $(ELFFILE)

include $(RIOTBASE)/Makefile.include
