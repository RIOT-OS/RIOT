APPLICATION = dev_ble
include ../Makefile.tests_common

BOARD_WHITELIST = yunjia-nrf51822

ifneq (,$(filter yunjia-nrf51822,$(BOARD)))
USEMODULE += ng_ble
endif

USEMODULE += dev_ble_autoinit

include $(RIOTBASE)/Makefile.include
