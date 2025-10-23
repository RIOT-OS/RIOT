# name of the application
APPLICATION = registry_example_cli

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# required modules
# enable the shell to execute the registry CLI
USEMODULE += shell
USEMODULE += shell_cmds_default

# enable littlefs2 for the persistent storage of the registry
USEMODULE += littlefs2
# enable mtd to use it as the storage location
USEMODULE += mtd

# enable the string_path extension of the registry to
# identify configuration parameters using strings instead of pointers
USEMODULE +=  registry_string_path
# enable the vfs storage implementation to persistently store configuration
# parameters to a storage device such as the mtd
USEMODULE +=  registry_storage_vfs
# enable the rgb_led schema (sys/rgb_led) to demonstrate how to configure
# RGB LEDs as an example for the registry CLI
USEMODULE +=  registry_namespace_sys_rgb_led

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
