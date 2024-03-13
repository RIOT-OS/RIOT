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
# - UDP and Packet Buffers: 26 kB (Stack and during embassy polling ISR)
# - embassy_executor tasks: statically allocated at compile time (nightly feature)
CFLAGS += '-DTHREAD_STACKSIZE_MAIN=(THREAD_STACKSIZE_DEFAULT + 20000)'
CFLAGS += '-DISR_STACKSIZE=(THREAD_STACKSIZE_DEFAULT + 30000)'

# The name of crate (as per Cargo.toml package name, but with '-' replaced with '_')
APPLICATION_RUST_MODULE = rust_matter_light
BASELIBS += $(APPLICATION_RUST_MODULE).module

FEATURES_REQUIRED += rust_target

USEMODULE += saul
USEMODULE += saul_default
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += periph_gpio
USEMODULE += ztimer
USEMODULE += ztimer_msec
USEMODULE += ztimer_usec
USEMODULE += ps

# networking
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_icmpv6_error
USEMODULE += shell_cmd_gnrc_udp
USEMODULE += sock_udp
USEMODULE += sock_async
USEMODULE += sock_aux_local
USEMODULE += ipv6
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

include $(RIOTBASE)/Makefile.include
