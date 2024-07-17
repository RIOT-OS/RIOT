# name of your application
APPLICATION = riot_as_cli_tool

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Include packages that pull up and auto-init the link layer.
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_default
# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

USEMODULE += auto_init_sock_dns    # add fall-back DNS server
USEMODULE += sock_dns              # include DNS client
USEMODULE += gnrc_ipv6_nib_dns     # include RDNSS option handling

USEMODULE += nanocoap_sock
USEMODULE += nanocoap_vfs

USEMODULE += vfs_default

USEMODULE += shell
USEMODULE += shell_cmds_default

# export the whole fs to RIOT
CFLAGS += -DFS_NATIVE_DIR=\"/\"
# mount it as '/' in RIOT to match the host
CFLAGS += -DCONFIG_NATIVE_MOUNTPOINT=\"/\"
# use the current working directory as download destination
CFLAGS += -DCONFIG_NCGET_DEFAULT_DATA_DIR=\"/proc/self/cwd/\"

# terminate RIOT with main()
CFLAGS += -DCONFIG_CORE_EXIT_WITH_MAIN=1
# don't be verbose
CFLAGS += -DCONFIG_SKIP_BOOT_MSG=1

RIOT_TERMINAL = native
BOARD_WHITELIST = native native64

DEVELHELP ?= 1

include $(RIOTBASE)/Makefile.include
