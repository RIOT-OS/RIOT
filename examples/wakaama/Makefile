# name of your application
APPLICATION = wakaama

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# TinyDTLS only has support for 32-bit architectures ATM
BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno chronos \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 \
                   z1

BOARD_INSUFFICIENT_MEMORY := airfy-beacon b-l072z-lrwan1 bluepill calliope-mini \
                             cc2650-launchpad cc2650stk maple-mini \
                             microbit nrf51dongle nrf6310 nucleo32-f031 \
                             nucleo32-f042 nucleo32-f303 nucleo32-l031 nucleo-f030 \
                             nucleo-f070 nucleo-f072 nucleo-f103 nucleo-f302 nucleo-f334 \
                             nucleo-l053 nucleo-l073 opencm904 \
                             spark-core stm32f0discovery yunjia-nrf51822


# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and sUDP
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_sock_udp
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# NOTE: Required by object_float_sensor.c
USEMODULE += saul_reg
# NOTE: Uncomment to automatically add any supported devices to the lwm2m client
#USEMODULE += saul_default
# NOTE: Uncomment to automatically add all GPIO pins from the SAUL registry to the lwm2m client
#USEMODULE += saul_gpio

# NOTE: Load module for memory allocator
USEMODULE += memarray

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Specific the server URI  address (NOTE: Domain names not supported yet)
SERVER_URI ?= '"coap://[fd00:dead:beef::1]"'

ifneq (,$(SERVER_URI))
  CFLAGS += -DLWM2M_SERVER_URI=$(SERVER_URI)
endif

ifneq (,$(findstring coaps,$(SERVER_URI)))
  $(info Enabling tinyDTLS)
  # NOTE: Add the package for TinyDTLS
  USEPKG += tinydtls

  #  (temporary fix) TinyDTLS <= 0.8.6 requires around 426 bytes in RAM.
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(3*THREAD_STACKSIZE_DEFAULT\)

  #TinyDTLs (crypto.c) made use of pthread
  ifneq ($(BOARD),native)
    USEMODULE += pthread
  endif

  # NOTE: Those are taken from TinyDTLS. As the original Makefiles are
  #      overwitten is a good idea to preserve them here.
  CFLAGS += -DDTLSv12 -DWITH_SHA256

  # NOTE: This adds support for TLS_PSK_WITH_AES_128_CCM_8
  CFLAGS += -DDTLS_PSK

  # NOTE: Without ECC, this defaults to 100 for RIOT which is not enough.
  CFLAGS += -DDTLS_MAX_BUF=200

  # NOTE: The configuration for socket or non-socket  communication in TinyDTLS.
  # (This can be removed once PR #7615 is merged)
  CFLAGS += -DWITH_RIOT_GNRC

  # NOTE: This adds support for TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8; UNTESTED!!
  #CFLAGS += -DDTLS_ECC

  # NOTE: If enabled TinyDTLS' log are disabled (if memory is a issue).
  # WARNING: Sometimes the log leads to Stack pointer corrupted.
  #          The reason is not identified yet.
  #          If said issue appears, enable this line.
  #CFLAGS += -DNDEBUG

  # NOTE: Set the TinyDTLS log level
  # (see lwm2mconfig.h or log_t in dtls_debug.h in the TinyDTLS source
  # tree for possible values.)
  #CFLAGS += -DDTLS_LOG_LEVEL=DTLS_LOG_DEBUG
else
  # Any special variable required to take TinyDTLS place.
endif

# NOTE: Add the package for wakaama
USEPKG += wakaama

# NOTE: Enabled Wakaama debug log
#CFLAGS += -DLWM2M_WITH_LOGS

# NOTE: Use wakaama in client mode with bootstrapping enabled
CFLAGS += -DLWM2M_CLIENT_MODE -DLWM2M_BOOTSTRAP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
