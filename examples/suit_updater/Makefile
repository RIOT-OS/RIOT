APPLICATION = suit_updater

# This is the board we have tested so far, it might not work with
# all supported boards
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := calliope-mini chronos microbit msb-430 msb-430h \
                             nucleo32-f031 nucleo32-f042 nucleo32-f303 nucleo32-l031 \
                             nucleo-f030 nucleo-f070 nucleo-f072 nucleo-f103 nucleo-f302 \
                             nucleo-f334 nucleo-l053 spark-core stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sock_udp

# Modules used for this example
USEMODULE += ps
USEMODULE += ota_suit
USEMODULE += libcose_crypt_tweetnacl
USEMODULE += nanocoap_sock
USEMODULE += suit
USEMODULE += hashes
USEMODULE += ps
USEPKG += libcose

# This application requires the RIOT bootlaoder
FEATURES_REQUIRED += riotboot

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# We need a header file with the public key on it
BUILDDEPS += keys.h
CFLAGS += -I$(CURDIR)

# Set a custom channel if needed
ifneq (,$(filter cc110x,$(USEMODULE)))          # radio is cc110x sub-GHz
  DEFAULT_CHANNEL ?= 0
  CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
else
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    DEFAULT_CHANNEL ?= 5
    FLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    DEFAULT_CHANNEL ?= 26
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif

include $(RIOTBASE)/Makefile.include

# TODO keys should be generated in `bin` not here
keys.h: $(RIOTTOOLS)/firmware/pub.key
	cp $< pubkey
	xxd -i pubkey $@
	rm pubkey
