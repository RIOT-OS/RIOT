# name of your application
APPLICATION = zigbee-remote

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52840dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += luid

USEMODULE += periph_gpio
USEMODULE += periph_gpio_irq
USEMODULE += ws281x

USEPKG += zboss

# Speficy the required features that a board has to have
FEATURES_REQUIRED += periph_flashpage_in_address_space

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

CFLAGS += -DZB_ED_ROLE
include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
