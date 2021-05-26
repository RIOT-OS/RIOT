# name of your application
APPLICATION = hcsr04

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(RIOTBASE)

USEMODULE += hcsr04
USEMODULE += saul_default

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

CFLAGS += -DHCSR04_TRIGGER_PIN=2
CFLAGS += -DHCSR04_ECHO_PIN=4

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
