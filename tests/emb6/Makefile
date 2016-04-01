APPLICATION = emb6

FEATURES_REQUIRED = periph_gpio periph_spi  # for at86rf231

BOARD ?= samr21-xpro

RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := msb-430h stm32f0discovery weio z1

USEMODULE += emb6_router
USEMODULE += emb6_conn_udp
USEMODULE += ipv6_addr
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += od

# define the driver to be used for selected boards
ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox,$(BOARD)))
  DRIVER := at86rf231
endif
ifneq (,$(filter mulle,$(BOARD)))
  DRIVER := at86rf212b
endif

# use the at86rf231 as fallback device
DRIVER ?= at86rf231

# include the selected driver
USEMODULE += $(DRIVER)

QUIET ?= 1

include $(RIOTBASE)/Makefile.include
