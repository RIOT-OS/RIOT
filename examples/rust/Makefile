# Name of your application.
APPLICATION = rust

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# The boards needs to support rust.
FEATURES_REQUIRED += rust_support

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# Since we built rust with `#![no_std]` we need libcore.
USEPKG += rust/libcore

# Module dependencies.
USEMODULE += rust/common # Provides lang_items
USEMODULE += rust/fmt    # Wrapper for sys/fmt
USEMODULE += fmt

include $(RIOTBASE)/Makefile.include
