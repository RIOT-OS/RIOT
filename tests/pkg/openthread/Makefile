APPLICATION = openthread

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

BOARD_WHITELIST := samr21-xpro iotlab-m3 fox iotlab-a8-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEPKG += openthread
USEMODULE += openthread_contrib
USEMODULE += libmbedcrypto
USEMODULE += libopenthread
USEMODULE += libopenthread-cli
USEMODULE += xtimer

ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox iotlab-a8-m3,$(BOARD)))
  DRIVER := at86rf231
endif

ifneq (,$(filter at86rf2%,$(DRIVER)))
  FEATURES_REQUIRED += periph_spi
  FEATURES_REQUIRED += periph_gpio
endif

USEMODULE += $(DRIVER)

USEMODULE += random
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += ipv6_addr

#required for C++ compiling
CXXEXFLAGS += -fno-rtti
USEMODULE += cpp11-compat

#Define PANID and CHANNEL used by default
#CFLAGS += -DOPENTHREAD_PANID=0xbeef -DOPENTHREAD_CHANNEL=11

include $(TESTBASE)/../Makefile.include
