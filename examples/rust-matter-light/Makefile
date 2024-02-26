# name of your application
APPLICATION = rust-matter-light

# If no BOARD is found in the environment, use this default:
#BOARD ?= nucleo-f103rb
BOARD ?= nucleo-f429zi

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# The name of crate (as per Cargo.toml package name, but with '-' replaced with '_')
APPLICATION_RUST_MODULE = rust_matter_light
BASELIBS += $(APPLICATION_RUST_MODULE).module

FEATURES_REQUIRED += rust_target

USEMODULE += saul
USEMODULE += saul_default
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += periph_gpio
USEMODULE += ztimer_msec
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

# All Rust components RIOT uses work on stable Rust. If any extra libraries
# were to require a more recent version, switch to `CARGO_CHANNEL =
# $(CARGO_CHANNEL_NIGHTLY)` to use whichever nightly version is available.
CARGO_CHANNEL ?= $(CARGO_CHANNEL_NIGHTLY)

# Currently unknown, something related to the LED_PORT definition that doesn't
# pass C2Rust's transpilation
BOARD_BLACKLIST := ek-lm4f120xl

include $(RIOTBASE)/Makefile.include
