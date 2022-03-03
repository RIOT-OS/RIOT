BOARD ?= esp32-ethernet-kit-v1_0

include ../Makefile.tests_common

USEMODULE += test_utils_netdev_eth_minimal

# the driver to test
USEMODULE += esp_eth
FEATURES_REQUIRED += arch_esp32
FEATURES_REQUIRED += periph_eth

INCLUDES += -I$(APPDIR)

include $(RIOTBASE)/Makefile.include
