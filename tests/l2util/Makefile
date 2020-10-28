DEVELHELP = 0

include ../Makefile.tests_common

USEMODULE += embunit
USEMODULE += l2util

# defining the "module" path defines this way in the hopes to make it more
#clearer.
CHECKED_IFDEF_PATHS = cc110x \
                      esp_now \
                      netdev_eth \
                      netdev_ieee802154 \
                      xbee \
                      nrfmin \
                      nimble_netif

CFLAGS += $(foreach path,$(CHECKED_IFDEF_PATHS),\
            -DMODULE_$(shell echo $(path) | tr a-z A-Z))

include $(RIOTBASE)/Makefile.include
