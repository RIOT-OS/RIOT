APPLICATION = wolf_client

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# BOARD_WHITELIST := To Be Determined
# BOARD_BLACKLIST := To Be Determined
# BOARD_INSUFFICIENT_MEMORY := To Be Determined

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

#CFLAGS += ??

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# May need these for seeding wolfCrypt DRBG
#USEMODULE += random
#USEMODULE += prng_minstd

USEPKG += wolfssl


include $(RIOTBASE)/Makefile.include
