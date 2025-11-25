# name of your application
APPLICATION = rust_gcoap

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

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

# for the "vfs" feature of riot-coap-handler-demos (and vfs.c)
USEMODULE += vfs
USEMODULE += constfs

# for the "saul" feature of riot-coap-handler-demos
USEMODULE += saul_default

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Add 8k extra stack: The Rust examples take more of it than gcoap expects,
# presumably because the example use the standard library's sting formatting
# instead of one of the more optimized formatters.
CFLAGS += -DGCOAP_STACK_SIZE='(THREAD_STACKSIZE_DEFAULT+DEBUG_EXTRA_STACKSIZE+sizeof(coap_pkt_t)+8192)'
# This thread needs some more stack for printing the addresses, once more being
# hit by string formatting.
CFLAGS += -DTHREAD_STACKSIZE_MAIN='(THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF+2048)'

# The name of crate (as per Cargo.toml package name, but with '-' replaced with '_')
APPLICATION_RUST_MODULE = rust_gcoap
BASELIBS += $(APPLICATION_RUST_MODULE).module

FEATURES_REQUIRED += rust_target

# Currently unknown, something related to the LED_PORT definition that doesn't
# pass C2Rust's transpilation
BOARD_BLACKLIST := ek-lm4f120xl

include $(RIOTBASE)/Makefile.include
