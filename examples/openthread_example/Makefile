APPLICATION = openthread_example

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP -Wall

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE+=openthread
USEMODULE+=openthread_cli

#USEMODULE += at86rf233
USEMODULE += at86rf231
#USEMODULE += kw2xrf

USEMODULE+=random
USEMODULE+=hashes
USEMODULE+=crypto
USEMODULE+=cipher_modes

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += ipv6_addr

FEATURES_REQUIRED += periph_i2c

include $(RIOTBASE)/Makefile.include
