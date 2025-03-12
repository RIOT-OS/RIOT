include ../Makefile.net_common

USEMODULE += embunit

USEMODULE += auto_init_gnrc_netif
USEMODULE += fmt
USEMODULE += ipv4_addr
USEMODULE += ipv6_addr
USEMODULE += gcoap_dns
USEMODULE += gnrc_ipv6_default
USEMODULE += netdev_default
USEMODULE += od
USEMODULE += shell
USEMODULE += shell_cmds_default

COAPS ?= 1

ifeq (1,$(COAPS))
  USEMODULE += gcoap_dtls
  USEMODULE += prng_sha256prng
  USEPKG += tinydtls
  KCONFIG_ADD_CONFIG += $(CURDIR)/coaps.config
endif

CFLAGS += -DTEST_SUITES
CFLAGS += -DHAS_SOCK_DNS_MOCK=1

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter native32 native64,$(BOARD)))
test: PORT=
$(call target-export-variables,test,PORT)
endif
