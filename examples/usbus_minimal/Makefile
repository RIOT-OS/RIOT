# name of your application
APPLICATION = usbus_minimal

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

USEMODULE += usbus
USEMODULE += auto_init_usbus
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
