# name of your application
APPLICATION = rust

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

#
FEATURES_REQUIRED += rust_support

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# use this to enable rust support.
# the current directory will be built using xargo.
CARGO_BUILD=1

include $(RIOTBASE)/Makefile.include
