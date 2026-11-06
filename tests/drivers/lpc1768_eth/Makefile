BOARD ?= seeeduino_arch-pro

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_eth_minimal

# the driver to test
USEMODULE += lpc1768_eth

# uncomment to enable additional features
# USEMODULE += lpc1768_eth_link_up
# USEMODULE += lpc1768_eth_auto

FEATURES_REQUIRED += periph_eth
FEATURES_REQUIRED += cpu_lpc1768

INCLUDES += -I$(APPDIR)

include $(RIOTBASE)/Makefile.include
