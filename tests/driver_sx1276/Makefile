APPLICATION = driver_sx1276
include ../Makefile.tests_common

FEATURES_REQUIRED = periph_spi periph_gpio

DISABLE_MODULE += auto_init
BOARD := samr21-xpro

USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

DRIVER ?= sx1276

# include the selected driver
USEMODULE += $(DRIVER)

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
