BOARD ?= same54-xpro

include ../Makefile.drivers_common

USEMODULE += test_utils_netdev_eth_minimal

# the driver to test
USEMODULE += sam0_eth
FEATURES_REQUIRED += periph_eth
FEATURES_REQUIRED += cpu_samd5x # TODO: complete with other SAM0 CPUs that have ethernet

INCLUDES += -I$(APPDIR)

SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
