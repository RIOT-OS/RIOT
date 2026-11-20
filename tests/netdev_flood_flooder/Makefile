BOARD ?= samr21-xpro

include ../Makefile.tests_common

# currently only the following network devices are supported by this test
#   - at86rf2xx
# so only whitelist boards that have these devices on-board
BOARD_WHITELIST = fox iotlab-m3 iotlab-a8-m3 mulle samr21-xpro samr30-xpro

DISABLE_MODULE += auto_init

USEMODULE += netdev_default
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
