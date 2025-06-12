# name of your application
APPLICATION = hello-world

# Change this to your board if you want to build for a different board
BOARD ?= feather-nrf52840-sense

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# This board requires a start sleep to actually catch the printed output
USEMODULE += shell

# Add the gpio module to the build
USEMODULE += periph_gpio
USEMODULE += periph_gpio_irq

# Enable the milliseconds timer.
USEMODULE += ztimer
USEMODULE += ztimer_msec

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
