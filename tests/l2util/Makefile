DEVELHELP = 0

include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := blackpill bluepill

USEMODULE += embunit
USEMODULE += l2util

# defining the "module" path defines this way in the hopes to make it more
#clearer.
CHECKED_IFDEF_PATHS = cc110x \
                      esp_now \
                      netdev_eth \
                      netdev_ieee802154 \
                      xbee \
                      nordic_softdevice_ble \
                      nrfmin

CFLAGS += $(foreach path,$(CHECKED_IFDEF_PATHS),\
            -DMODULE_$(shell echo $(path) | tr a-z A-Z))

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
