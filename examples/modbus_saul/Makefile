# name of your application
APPLICATION = modbus_saul

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# we want to use SAUL:
USEMODULE += modbus
USEMODULE += modbus_rtu
USEMODULE += saul_default
USEMODULE += shell_cmds_default

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
