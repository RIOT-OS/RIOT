# name of your application
APPLICATION = dtls_echo

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# TinyDTLS only has support for 32-bit architectures ATM
BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno chronos \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 \
                   z1

BOARD_INSUFFICIENT_MEMORY := airfy-beacon calliope-mini cc2650stk maple-mini \
                             microbit nrf51dongle nrf6310 nucleo32-f031 \
                             nucleo32-f042 nucleo32-f303 nucleo32-l031 nucleo-f030 \
                             nucleo-f070 nucleo-f072 nucleo-f103 nucleo-f302 nucleo-f334 \
                             nucleo-l053 nucleo-l073 opencm904 pca10000 pca10005 \
                             spark-core stm32f0discovery weio yunjia-nrf51822

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
USEMODULE += gnrc_sock_udp
# Add a routing protocol (optional)
USEMODULE += gnrc_rpl

# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps


# Uncomment to redefine port. Default: DTLS port (20220). Useful for FEBx range
#DTLS_PORT = 20220
#CFLAGS += -DDTLS_DEFAULT_PORT=$(DTLS_PORT)

# If enabled, DTLS client will spoofed a CoAP message (GET /riot/value)
#CFLAGS += -DCOAP_MSG_SPOOF


# NOTE: The following lines are custom configuration for TinyDTLS

USEPKG += tinydtls

# This adds support for TLS_PSK_WITH_AES_128_CCM_8
CFLAGS += -DDTLS_PSK
# This adds support for TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
#CFLAGS += -DDTLS_ECC

# If enabled TinyDTLS' logs are disabled (if memory is an issue).
#CFLAGS += -DNDEBUG

# NOTE: Although dtls-echo can run with default value. The use of ECC curves
#       seems to require more memory for the threads.
# TODO: A more deep memory evaluation is required
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

# NOTE: The configuration for socket or non-socket  communication in TinyDTLS.
CFLAGS += -DWITH_RIOT_GNRC


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
