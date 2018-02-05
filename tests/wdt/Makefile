# name of your application
APPLICATION = default

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# include and auto-initialize all available sensors
USEMODULE += auto_init
USEMODULE += xtimer

FEATURES_REQUIRED += periph_wdt

include $(RIOTBASE)/Makefile.include

ifeq ("$(BOARD)", "remote-revb")
CFLAGS+=-DWDT_GPIO_DEBUG
CFLAGS+=-DREBOOT_GPIO="GPIO_PIN(3,2)"
CFLAGS+=-DWDT_RESET_GPIO="GPIO_PIN(3,0)"
CFLAGS+=-DWDT_BLOCK_GPIO="GPIO_PIN(0,7)"
endif

