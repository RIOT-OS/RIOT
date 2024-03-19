BOARD ?= esp32-ethernet-kit-v1_0

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_eth_minimal

# the driver to test
USEMODULE += esp_eth
FEATURES_REQUIRED += arch_esp32

INCLUDES += -I$(APPDIR)

SHOULD_RUN_KCONFIG =?

include $(RIOTBASE)/Makefile.include
