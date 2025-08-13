# name of your application
APPLICATION = rust_gcoap

# TODO: Fix bug in docker image
BOARD_BLACKLIST := native32 native64 native

# The name of crate (as per Cargo.toml package name, but with '-' replaced with '_')
#
# The presence of this triggers building Rust code contained in this
# application in addition to any C code.
APPLICATION_RUST_MODULE = rust_gcoap
APPLICATION_C_SRCS += assert_stub.c


# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Basic networking, and gcoap
USEMODULE += gcoap
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo

USEMODULE += ztimer
USEMODULE += ztimer_usec
USEMODULE += ztimer_msec
USEMODULE += ztimer_sec

USEMODULE += gnrc_netapi_callbacks

# for the "vfs" feature of riot-coap-handler-demos (and vfs.c)
USEMODULE += vfs
USEMODULE += constfs

# for the "saul" feature of riot-coap-handler-demos
USEMODULE += saul_default

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#DEVELHELP ?= 1
# in your Application’s Makefile (next to APPLICATION, etc.)
# add this line:
CFLAGS += -DNDEBUG


# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Add 12k extra stack: The Rust examples take more of it than gcoap expects,
# for reasons that are not fully understood (it's not the string formatter).
CFLAGS += -DGCOAP_STACK_SIZE='(THREAD_STACKSIZE_DEFAULT+DEBUG_EXTRA_STACKSIZE+sizeof(coap_pkt_t)+12288)'
# This thread needs some more stack for printing the addresses, once more being
# hit by string formatting.
# in your Application Makefile, replace your THREAD_STACKSIZE_MAIN override with something like:
CFLAGS += -DTHREAD_STACKSIZE_MAIN='(THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF + 32768)'


include $(RIOTBASE)/Makefile.include


DEFAULT_CHANNEL ?= 16

include $(RIOTMAKE)/default-radio-settings.inc.mk
