APPLICATION = openthread_example

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP -Wall

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEPKG=openthread
USEMODULE+=openthread_contrib
USEMODULE+=openthread_core
USEMODULE+=openthread_core_common
USEMODULE+=openthread_core_coap
USEMODULE+=openthread_core_thread
USEMODULE+=openthread_core_net
USEMODULE+=openthread_core_crypto
USEMODULE+=openthread_core_mac
USEMODULE+=openthread_cli
USEMODULE+=openthread_contrib_netdev2

USEMODULE+=mbedtls
USEMODULE+=mbedtls_lib
#USEMODULE+=gnrc_netdev_default
USEMODULE+=netstats_l2


USEMODULE+=xtimer
USEMODULE += at86rf233
USEMODULE+=random
USEMODULE+=hashes
USEMODULE+=crypto
USEMODULE+=cipher_modes

FEATURES_REQUIRED += cpp
FEATURES_REQUIRED += periph_spi periph_gpio


#USEMODULE += gnrc_netdev_default
#USEMODULE += auto_init_gnrc_netif

include $(RIOTBASE)/Makefile.include
