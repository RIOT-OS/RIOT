BOARD ?= slstk3701a

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_eth_minimal

# the driver to test
USEMODULE += efm32_eth

# uncomment to enable additional features
# USEMODULE += efm32_eth_link_up
# USEMODULE += efm32_eth_auto

FEATURES_REQUIRED += periph_eth
FEATURES_REQUIRED += cpu_efm32

INCLUDES += -I$(APPDIR)

include $(RIOTBASE)/Makefile.include
