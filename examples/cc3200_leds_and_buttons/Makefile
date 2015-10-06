# name of your application
APPLICATION = leds_and_buttons

# If no BOARD is found in the environment, use this default:
BOARD ?= cc3200-launchxl

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include

# TI cc3200-sdk is not compliant with following RIOT constraints
CFLAGS += -Wno-strict-prototypes -Wno-old-style-definition
