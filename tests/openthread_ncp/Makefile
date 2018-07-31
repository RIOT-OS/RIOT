APPLICATION = openthread_ncp

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# These are the boards that OpenThread stack has been tested on
BOARD_WHITELIST := samr21-xpro iotlab-m3 fox iotlab-a8-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP -Wall

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# NCP: A Network Co-Processor is used with wpantund software. wpantund is a user-space network
# interface driver/daemon that provides a native IPv6 network interface to a low-power wireless Network Co-Processor
# FTD (see openthread_test_ftd): NCP needs Full Thread Device to act as a router in the network
USEPKG += openthread
USEMODULE += openthread-ftd
USEMODULE += openthread-ncp-ftd

ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox iotlab-a8-m3,$(BOARD)))
  DRIVER := at86rf231
endif

ifneq (,$(filter at86rf2%,$(DRIVER)))
  FEATURES_REQUIRED += periph_spi periph_gpio
endif

USEMODULE += $(DRIVER)

USEMODULE += xtimer
USEMODULE += random
USEMODULE += ps

#required for C++ compiling
CXXEXFLAGS += -fno-rtti
USEMODULE += cpp11-compat

include $(RIOTBASE)/Makefile.include
