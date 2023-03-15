BOARD ?= samr21-xpro
include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += usbus_cdc_ecm
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

# Boards that don't have enough endpoints to use CDC ACM together with CDC ECM
BOARD_BLACKLIST += \
  stm32f429i-disco \
  stm32f4discovery \
  weact-f401cc \
  weact-f401ce \
  weact-f411ce \
  #

include $(RIOTBASE)/Makefile.include
