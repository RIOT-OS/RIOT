# name of your application
APPLICATION = dtls_echo

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

BOARD_BLACKLIST := z1 wsn430-v1_4 wsn430-v1_3b waspmote-pro arduino-mega2560  \
                   msb-430h msb-430 chronos telosb msbiot cc2538dk \
                   saml21-xpro samr21-xpro arduino-duemilanove arduino-uno \
                   nrf52dk # see #6022

BOARD_INSUFFICIENT_MEMORY := airfy-beacon chronos msb-430 msb-430h nrf51dongle \
                          nrf6310 nucleo-f103 nucleo-f334 pca10000 pca10005 spark-core \
                          stm32f0discovery telosb weio wsn430-v1_3b wsn430-v1_4 \
                          yunjia-nrf51822 z1 nucleo-f072 cc2650stk nucleo-f030 \
                          nucleo-f070

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and sUDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
# This application dumps received packets to STDIO using the pktdump module
USEMODULE += gnrc_pktdump
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

#TinyDTLs (crypto.c) made use of pthread
 ifneq ($(BOARD),native)
 USEMODULE += pthread
 endif

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# NOTE: Add the package for TinyDTLS
USEPKG += tinydtls

# NOTE: Those are taken from TinyDTLS. As the original Makefiles are
#      overwitten is a good idea to preserve them here.
CFLAGS += -DDTLSv12 -DWITH_SHA256

# NOTE: This adds support for TLS_PSK_WITH_AES_128_CCM_8
CFLAGS += -DDTLS_PSK

# NOTE: This adds support for TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
CFLAGS += -DDTLS_ECC

# NOTE: If enabled TinyDTLS' log are disabled (if memory is a issue).
# WARNING: Sometimes the log leads to Stack pointer corrupted.
#          The reason is not identified yet.
#          If said issue appears, enable this line.
#CFLAGS += -DNDEBUG

# NOTE: The configuration for socket or non-socket  communication in TinyDTLS.
CFLAGS += -DWITH_RIOT_GNRC

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
