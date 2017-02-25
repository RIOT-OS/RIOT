APPLICATION = driver_kw2xrf
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

BOARD_INSUFFICIENT_MEMORY := stm32f0discovery nucleo-f334 weio

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_netdev_default
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

DRIVER ?= kw2xrf

# finally include the actual chosen driver
USEMODULE += $(DRIVER)

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
