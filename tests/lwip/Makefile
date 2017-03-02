APPLICATION = lwip
# overwrite board, do not set native as default
BOARD ?= iotlab-m3
include ../Makefile.tests_common

RIOTBASE ?= $(CURDIR)/../..

BOARD_BLACKLIST := arduino-mega2560 msb-430h telosb waspmote-pro z1 arduino-uno \
                   arduino-duemilanove msb-430 wsn430-v1_4 wsn430-v1_3b
BOARD_INSUFFICIENT_MEMORY := airfy-beacon arduino-mega2560 msb-430h nrf6310 \
                             nucleo-f334 pca10005 stm32f0discovery telosb \
                             weio yunjia-nrf51822 z1 nucleo-f030 nucleo-f072 \
                             nucleo32-f031 nucleo-l053

# including lwip_ipv6_mld would currently break this test on at86rf2xx radios
USEMODULE += lwip lwip_ipv6_autoconfig lwip_conn_ip lwip_netdev2
USEMODULE += lwip_udp lwip_conn_udp
USEMODULE += ipv6_addr
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += od

# use the at86rf231 as fallback device
DRIVER := at86rf231

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
ifneq (,$(filter native,$(BOARD)))
  DRIVER := netdev2_tap
  USEMODULE += lwip_ethernet
endif

ifneq (,$(filter at86rf2%,$(DRIVER)))
  FEATURES_REQUIRED = periph_spi periph_gpio
endif

USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
