# name of the application
APPLICATION = registry_example_cli

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# required modules
USEMODULE += shell
USEMODULE += shell_cmds_default

USEMODULE += littlefs2
USEMODULE += mtd

USEMODULE +=  registry_string_path
USEMODULE +=  registry_storage_vfs
USEMODULE +=  registry_namespace_sys_rgb_led
USEMODULE +=  registry_namespace_tests_full
USEMODULE +=  registry_namespace_tests_nested

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
