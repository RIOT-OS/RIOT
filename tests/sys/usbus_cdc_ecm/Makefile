BOARD ?= samr21-xpro
include ../Makefile.sys_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += usbus_cdc_ecm
USEMODULE += shell_cmds_default
USEMODULE += ps

# Boards that don't have enough endpoints to use CDC ACM together with CDC ECM
ifeq (,$(filter stdio_%,$(filter-out stdio_cdc_acm,$(USEMODULE))))
  BOARD_BLACKLIST += \
    sipeed-longan-nano \
    sipeed-longan-nano-tft \
    seeedstudio-gd32 \
    stm32f4discovery \
    weact-f401cc \
    weact-f401ce \
    weact-f411ce \
    #
endif

define _usbus_cdc_ecm_blacklist_info
Warning:
    The board is blacklisted because it uses `stdio_cdc_acm` as STDIO, which needs
    a CDC ACM interface in addition to the CDC ECM interface, but the number of
    available endpoints is not sufficient for this. To use this application you
    have to use `stdio_uart` or any other `stdio_*` module, for example:

    USEMODULE=stdio_uart BOARD=$(BOARD) make -C tests/sys/usbus_cdc_ecm

endef

ifneq (,$(filter $(BOARD),$(BOARD_BLACKLIST)))
  $(info $(_usbus_cdc_ecm_blacklist_info))
endif

include $(RIOTBASE)/Makefile.include
