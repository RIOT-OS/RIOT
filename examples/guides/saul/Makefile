# name of your application
APPLICATION = saul_example

# Change this to your board if you want to build for a different board
BOARD ?= arduino-feather-nrf52840-sense

# This has to be the absolute path to the RIOT base directory:
# If you are following the tutorial, your RIOT base directory will
# most likely be something like RIOTBASE ?= $(CURDIR)/RIOT
# instead of this
RIOTBASE ?= $(CURDIR)/../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# This board requires a start sleep to actually catch the printed output
USEMODULE += shell

# Add the SAUL module to the application
USEMODULE += saul
USEMODULE += saul_default

# Enable the milliseconds timer.
USEMODULE += ztimer
USEMODULE += ztimer_msec

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
