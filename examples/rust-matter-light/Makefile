# name of your application
APPLICATION = rust-matter-light

# If no BOARD is found in the environment, use this default:
BOARD ?= native64

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Estimated minimum usage (critical parts):
# - Matter service: 18 kB
# - Packet Buffers: 24 kB
# - UDP Buffers: 3 kB
CFLAGS += '-DTHREAD_STACKSIZE_MAIN=(THREAD_STACKSIZE_DEFAULT + 50000)'
CFLAGS += '-DISR_STACKSIZE=2048'
CFLAGS += '-DGNRC_NETIF_MSG_QUEUE_SIZE=16'
CFLAGS += '-DCONFIG_GNRC_SOCK_MBOX_SIZE_EXP=8'

# The name of crate (as per Cargo.toml package name, but with '-' replaced with '_')
APPLICATION_RUST_MODULE = rust_matter_light
BASELIBS += $(APPLICATION_RUST_MODULE).module

FEATURES_REQUIRED += rust_target

# Use RTC for sys_epoch, otherwise ztimer
FEATURES_OPTIONAL += periph_rtc

# Comment this line in to activate storing & loading runtime configurations
# NOTE: To use this feature, a PersistenceManager (in 'src/persist.rs') must be implemented for the target platform
#CARGO_OPTIONS += --features psm

USEMODULE += saul
USEMODULE += saul_default
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += shell_cmd_heap
USEMODULE += ztimer
USEMODULE += ztimer_msec
USEMODULE += ztimer_usec
USEMODULE += ztimer_sec
USEMODULE += ps
USEMODULE += random
USEMODULE += periph_hwrng
USEMODULE += prng_sha256prng
USEMODULE += vfs_default
USEMODULE += constfs

# networking
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_icmpv6_error
USEMODULE += sock_udp
USEMODULE += sock_async
USEMODULE += sock_aux_local

include $(RIOTBASE)/Makefile.include
