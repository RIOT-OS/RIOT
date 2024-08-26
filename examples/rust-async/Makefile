# name of your application
APPLICATION = rust-async

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Some timers for the example
USEMODULE += ztimer_msec

# Required by the async executor
USEMODULE += core_thread_flags

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# The name of crate (as per Cargo.toml package name, but with '-' replaced with '_')
APPLICATION_RUST_MODULE = rust_async
BASELIBS += $(APPLICATION_RUST_MODULE).module

FEATURES_REQUIRED += rust_target

include $(RIOTBASE)/Makefile.include
