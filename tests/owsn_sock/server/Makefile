APPLICATION = owsn_sock_server_test

RIOTBASE ?= $(CURDIR)/../../..
include ../../Makefile.tests_common

BOARD ?= iotlab-m3

USEPKG += openwsn
USEMODULE += ipv6_addr
USEMODULE += ps
USEMODULE += od_string
USEMODULE += random
USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += sock_udp
USEMODULE += owsn
USEMODULE += owsn_sock
USEMODULE += owsn_sock_udp
USEMODULE += nanocoap_sock


# Enable this to use single channel debugging mode
#CFLAGS=-DOW_RIOT_SINGLE_CHANNEL=26

CFLAGS += -DDEVELHELP
WERROR = 0

ifneq (,$(filter nucleo-f103,$(BOARD)))
    USEMODULE += at86rf233

    CFLAGS += -DAT86RF2XX_PARAM_SPI=SPI_DEV\(0\)
    CFLAGS += -DAT86RF2XX_PARAM_SPI_CLK=SPI_CLK_5MHZ
    CFLAGS += -DAT86RF2XX_PARAM_CS=GPIO_PIN\(1,1\)
    CFLAGS += -DAT86RF2XX_PARAM_INT=GPIO_PIN\(2,4\)
    CFLAGS += -DAT86RF2XX_PARAM_SLEEP=GPIO_PIN\(1,15\)
    CFLAGS += -DAT86RF2XX_PARAM_RESET=GPIO_PIN\(1,14\)
endif

include $(RIOTBASE)/Makefile.include
