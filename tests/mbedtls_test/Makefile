APPLICATION= lwip_mbedtls_test

RIOTBASE ?= $(CURDIR)/../..

BOARD ?= native

USEPKG += mbedtls

USEMODULE += netdev_default
USEMODULE += mbedtls_entropy
USEMODULE += ipv4_addr
USEMODULE += lwip_arp
USEMODULE += lwip_ipv4
USEMODULE += lwip_ipv6
USEMODULE += lwip_dhcp_auto
USEMODULE += lwip lwip_netdev
USEMODULE += lwip_udp
USEMODULE += lwip_tcp
USEMODULE += sock_tcp

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# tinydtls needs crypto secure PRNG
USEMODULE += prng_sha1prng
USEMODULE += random

CFLAGS += -DTHREAD_STACKSIZE_MAIN="(5 * THREAD_STACKSIZE_LARGE + THREAD_EXTRA_STACKSIZE_PRINTF )"
CFLAGS += -DCONFIG_ENTROPY_SOURCE_ADC_HMIN="200"
CFLAGS += -DISR_STACK_SIZE=4096
CFLAGS += -DLWIP_DEBUG_ENABLED=0
CFLAGS += -DMBEDTLS_ENABLED=1
CFLAGS += -D_SOCKLEN_T_DECLARED=1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
