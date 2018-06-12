APPLICATION = openthread

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# These are the boards that OpenThread stack has been tested on
BOARD_WHITELIST := samr21-xpro iotlab-m3 fox iotlab-a8-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEPKG += openthread
OPENTHREAD_TYPE ?= ftd
ifeq ($(OPENTHREAD_TYPE),mtd)
  # MTD: A Minimal Thread Device does not have router functionality
  # compiled in. As a result, it is not necessary to configure the
  # routerrole on an MTD. At the same time, an MTD may or may not be sleepy.
  USEMODULE += openthread-mtd
  USEMODULE += openthread-cli-mtd
else
  # ftd: A Full Thread Device has router functionality compiled in
  USEMODULE += openthread-ftd
  USEMODULE += openthread-cli-ftd
endif

#Define PANID, CHANNEL and UART_BAUDRATE used by default
OPENTHREAD_PANID ?= 0xbeef
OPENTHREAD_CHANNEL ?= 26

CFLAGS += -DOPENTHREAD_PANID=$(OPENTHREAD_PANID)
CFLAGS += -DOPENTHREAD_CHANNEL=$(OPENTHREAD_CHANNEL)

ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox iotlab-a8-m3,$(BOARD)))
  DRIVER := at86rf231
endif

USEMODULE += $(DRIVER)

USEMODULE += xtimer
USEMODULE += random
USEMODULE += ps

#required for C++ compiling
CXXEXFLAGS += -fno-rtti
USEMODULE += cpp11-compat

include $(RIOTBASE)/Makefile.include
