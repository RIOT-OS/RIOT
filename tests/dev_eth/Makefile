APPLICATION = dev_eth
include ../Makefile.tests_common

BOARD_WHITELIST = native
FEATURES_REQUIRED += ethernet

ifneq (,$(filter native,$(BOARD)))
USEMODULE += ng_nativenet
endif

USEMODULE += dev_eth_autoinit

include $(RIOTBASE)/Makefile.include
