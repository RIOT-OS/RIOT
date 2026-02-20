# name of the application
APPLICATION = registry_example_core

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# required modules
# enable board_led schema (sys/board_led) of the riot registry
USEMODULE += registry_namespace_sys_board_led
# enable the ztimer to turn the led on and off every second
USEMODULE += ztimer_msec

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
