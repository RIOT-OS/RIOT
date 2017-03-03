# name of your application
APPLICATION = gnrc_border_router

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := airfy-beacon cc2650stk maple-mini msb-430 msb-430h pca10000 \
                             pca10005 nrf51dongle nrf6310 nucleo-f103 nucleo-f334 \
                             spark-core stm32f0discovery telosb \
                             weio wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1 nucleo-f072 \
                             nucleo-f030 nucleo-f070 microbit calliope-mini \
                             nucleo32-f042 nucleo32-f303 opencm904 nucleo32-f031 \
                             nucleo-l073 nucleo-l053

BOARD_BLACKLIST += mips-malta # No UART available.

# use ethos (ethernet over serial) for network communication and stdio over
# UART, but not on native, as native has a tap interface towards the host.
ifeq (,$(filter native,$(BOARD)))
GNRC_NETIF_NUMOF := 2
USEMODULE += ethos gnrc_netdev2

# ethos baudrate can be configured from make command
ETHOS_BAUDRATE ?= 115200
CFLAGS += '-DETHOS_UART=UART_DEV(0)' -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE) -DUSE_ETHOS_FOR_STDIO
FEATURES_REQUIRED += periph_uart
endif

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for 6LoWPAN border router
USEMODULE += gnrc_sixlowpan_border_router_default
# Add forwarding table
USEMODULE += fib
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# include UHCP client
USEMODULE += gnrc_uhcpc

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1
TAP ?= tap0
IPV6_PREFIX ?= 2001:db8::/64

include $(RIOTBASE)/Makefile.include

.PHONY: host-tools

term: host-tools
	$(Q)sudo sh $(RIOTBASE)/dist/tools/ethos/start_network.sh $(PORT) $(TAP) $(IPV6_PREFIX)

host-tools:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTBASE)/dist/tools

# Set a custom channel if needed
ifneq (,$(filter cc110x,$(USEMODULE)))          # radio is cc110x sub-GHz
  DEFAULT_CHANNEL ?= 0
  CFLAGS += -DCC110X_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
else
  ifneq (,$(filter at86rf212b,$(USEMODULE)))    # radio is IEEE 802.15.4 sub-GHz
    DEFAULT_CHANNEL ?= 5
    CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=$(DEFAULT_CHANNEL)
  else                                          # radio is IEEE 802.15.4 2.4 GHz
    DEFAULT_CHANNEL ?= 26
    CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=$(DEFAULT_CHANNEL)
  endif
endif
