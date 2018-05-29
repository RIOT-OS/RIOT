# name of your application
APPLICATION = senml-json-example

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# the remote-* boards are blacklisted because RAM is insufficient for the unittests;
# all other boards are in this list because atof, atoi, etc. are not supported
BOARD_BLACKLIST := remote-pa remote-reva remote-revb chronos \
		   msb-430 msb-430h seeeduino_arch-pro telosb \
		   wsn430-v1_3b wsn430-v1_4 z1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS +=

USEPKG += jsmn
USEMODULE += base64
USEMODULE += senml

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
