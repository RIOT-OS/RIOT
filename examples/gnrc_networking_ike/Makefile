# name of your application
APPLICATION = gnrc_networking_ike

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
#USEMODULE += gnrc_icmpv6_error
USEMODULE += shell
USEMODULE += shell_cmds_default
#USEMODULE += ps

#USEMODULE += ssp


USEMODULE += sock_udp

USEMODULE += ike

USEPKG += wolfssl
#USEMODULE += wolfssl
USEMODULE += wolfcrypt
#USEMODULE += wolfcrypt_rsa
USEMODULE += wolfcrypt_dh
#USEMODULE += wolfssl_internal
#USEMODULE += wolfcrypt_random
#USEMODULE += wolfcrypt_ffdhe_2048 # for DHE
#USEMODULE += wolfssl_tls
#USEMODULE += wolfcrypt_ecc # for DHE

ifeq (esp8266-esp-12x,$(BOARD))
  # For ESP
  USEMODULE += esp_wifi
  USEMODULE += esp_idf_heap
  CFLAGS += -DESP_WIFI_SSID=\"TestNet\"
  CFLAGS += -DESP_WIFI_PASS=\"password\"
  CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=2048
else
  # For native / STM
  USEMODULE += netdev_default
endif


#CFLAGS += -Warray-bounds=0

CFLAGS_OPT = -Os
#CFLAGS_OPT = -O3
#CFLAGS_OPT = -g -ggdb3 -fno-omit-frame-pointer -O0
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT\)
#CFLAGS += -DISR_STACKSIZE=1024


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1


# Uncomment the following 2 lines to specify static link lokal IPv6 address
# this might be useful for testing, in cases where you cannot or do not want to
# run a shell with ifconfig to get the real link lokal address.
#IPV6_STATIC_LLADDR ?= '"fe80::cafe:cafe:cafe:1"'
#CFLAGS += -DGNRC_IPV6_STATIC_LLADDR=$(IPV6_STATIC_LLADDR)

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

EXTERNAL_BOARD_DIRS += $(RIOTBASE)/tests/external_board_dirs/esp-ci-boards

include $(RIOTBASE)/Makefile.include
