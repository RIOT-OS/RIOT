# name of your application
APPLICATION = gcoap_fileserver

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_cmds_default

# enable the fileserver module
USEMODULE += gcoap_fileserver
USEMODULE += nanocoap_fileserver_callback
USEMODULE += nanocoap_fileserver_delete
USEMODULE += nanocoap_fileserver_put

# select network modules
USEMODULE += gnrc_ipv6_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += netdev_default
USEMODULE += gnrc_icmpv6_echo

# enable default file system mount points
USEMODULE += vfs_default
# USEMODULE += vfs_auto_format

include $(RIOTBASE)/Makefile.include
