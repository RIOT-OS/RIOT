include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031 \
                             nucleo-f334r8 nucleo-l053 stm32f0discovery

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netdev_default
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DRIVER ?= kw2xrf

# finally include the actual chosen driver
USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include
