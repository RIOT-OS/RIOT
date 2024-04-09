# name of your application
APPLICATION = filesystem

# If no BOARD is found in the environment, use this default:
BOARD ?= native


# Blacklisting msp430-based boards, as file syscalls are not supported
FEATURES_BLACKLIST += arch_msp430

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

# Use the default file system
USEMODULE += vfs_default
USEMODULE += constfs
# USEMODULE += devfs

# Enable to automatically format if mount fails
#USEMODULE += vfs_auto_format

# For LittleFS on real devices, the main stack size has to be
# increased:
CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048

include $(RIOTBASE)/Makefile.include
