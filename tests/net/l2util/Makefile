DEVELHELP = 0

include ../Makefile.net_common

# needed for 'uppercase' below, as 'Makefile.include' is only included at the end
include $(RIOTBASE)/makefiles/utils/strings.mk

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
            -DMODULE_$(call uppercase,$(path)))

include $(RIOTBASE)/Makefile.include
