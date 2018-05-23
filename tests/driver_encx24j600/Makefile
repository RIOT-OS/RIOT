include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := msb-430 msb-430h \
                             nucleo-l053r8 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-l031k6 stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += auto_init_gnrc_netif
USEMODULE += encx24j600
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# lower pktbuf size
CFLAGS += -DGNRC_PKTBUF_SIZE=2048

# set board specific peripheral configurations
ifneq (,$(filter nucleo-f334r8,$(BOARD)))
# these settings are probably valid for PoEll-i on most nucelo boards, but
# tested only on nucleo-f334r8
  ENC_SPI ?= SPI_DEV\(0\)
  ENC_CS  ?= GPIO_PIN\(PORT_C,10\)
  ENC_INT ?= GPIO_PIN\(PORT_D,2\)
endif

# fallback: set SPI bus and pins to default values
ENC_SPI ?= SPI_DEV\(0\)
ENC_CS  ?= GPIO_PIN\(0,0\)
ENC_INT ?= GPIO_PIN\(0,1\)
# export SPI and pins
CFLAGS += -DENCX24J600_SPI=$(ENC_SPI)
CFLAGS += -DENCX24J600_CS=$(ENC_CS)
CFLAGS += -DENCX24J600_INT=$(ENC_INT)

# make sure we read the local encx24j600 params file
CFLAGS += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
