BOARD ?= nucleo-f207zg

INCLUDES += -I$(APPDIR)

include ../Makefile.tests_common

USEMODULE += test_utils_netdev_eth_minimal

# the driver to test
USEMODULE += stm32_eth

FEATURES_REQUIRED += periph_eth
FEATURES_REQUIRED += cpu_stm32

include $(RIOTBASE)/Makefile.include
