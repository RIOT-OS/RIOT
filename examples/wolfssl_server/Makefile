APPLICATION = wolf_server

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..


# If no BOARD is found in the environment, use this default:
BOARD ?= native
# lwIP's memory management doesn't seem to work on non 32-bit platforms at the
# moment.
BOARD_BLACKLIST := arduino-uno arduino-duemilanove arduino-mega2560 chronos \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b \
                   wsn430-v1_4 z1 jiminy-mega256rfr2 mega-xplained
BOARD_INSUFFICIENT_MEMORY = nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 nucleo-f030r8 \
                            nucleo-f303k8 nucleo-f334r8 nucleo-l053r8 \
                            stm32f0discovery

LWIP_IPV4 ?= 1

ifneq (0, $(LWIP_IPV4))
  USEMODULE += ipv4_addr
  USEMODULE += lwip_arp
  USEMODULE += lwip_ipv4
  CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
  LWIP_IPV6 ?= 0
else
  LWIP_IPV6 ?= 1
endif

ifneq (0, $(LWIP_IPV6))
  USEMODULE += ipv6_addr
  USEMODULE += lwip_ipv6_autoconfig
endif

USEMODULE += inet_csum
USEMODULE += lwip_netdev
USEMODULE += lwip_sock_tcp
USEMODULE += netdev_eth
USEMODULE += netdev_test
USEMODULE += ps

#DISABLE_MODULE += auto_init

#CFLAGS += ??

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# May need these for seeding wolfCrypt DRBG
#USEMODULE += random
#USEMODULE += prng_minstd

USEPKG += wolfssl
USEMODULE += wolfssl_socket wolfcrypt_rsa wolfcrypt_dh


include $(RIOTBASE)/Makefile.include
