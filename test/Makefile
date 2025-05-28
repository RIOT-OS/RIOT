# name of your application
APPLICATION = ttls_wolfssl

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT

# Include the TLS module which will pull in all dependencies
USEMODULE += tcp_tls
USEPKG += wolfssl

# Source files
SRC += main.c

# Comment this out to disable code in RIOT that does safety checking
DEVELHELP ?= 1

# Change this to 0 to show compiler invocation lines by default:
QUIET ?= 0

include $(RIOTBASE)/Makefile.include
