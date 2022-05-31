# If no BOARD is found in the environment, use this default:
BOARD ?= same54-xpro

include ../Makefile.tests_common

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# No interactive_sync
DISABLE_MODULE += test_utils_interactive_sync

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# include riotboot_vfs support
USEMODULE += riotboot_vfs
# select a filesystem
USEMODULE += vfs_default
# auto-format the file-system if needed
USEMODULE += vfs_auto_format

include $(RIOTBASE)/Makefile.include
