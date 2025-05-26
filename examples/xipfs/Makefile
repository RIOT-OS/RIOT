# name of your application
APPLICATION = xipfs

# If no BOARD is found in the environment, use this default:
BOARD ?= dwm1001

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

BLOBS += hello-world.fae

# Modules to include:
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += saul_default

# Use xipfs file system
USEMODULE += xipfs

CFLAGS +=-DRIOT_OS

include $(RIOTBASE)/Makefile.include
