APPLICATION = openthread

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# These are the boards that OpenThread stack has been tested on
BOARDS_SUPPORTED := \
                    samr21-xpro \
                    iotlab-m3 \
                    iotlab-a8-m3 \
                    frdm-kw41z \
                    openlabs-kw41z-mini-256kib \
                    openlabs-kw41z-mini \
                    phynode-kw41z \
                    usb-kw41z \
                    cc2538dk \
                    remote-reva \
                    remote-revb \
                    omote \
                    openmote-cc2538 \
                    nrf52840dk \
                    nrf52840-mdk \
                    adafruit-feather-nrf52840-sense \
                    reel \
                    #

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# (optional) Absolute path to expand the OpenThread config
# All options under https://openthread.io/reference - config variables
OT_PROJECT_CONFIG_PATH ?= $(CURDIR)/project_conf.h
export OT_PROJECT_CONFIG_PATH

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEPKG += openthread

# FTD: A Full Thread Device has router functionality compiled in
#
# MTD: A Minimal Thread Device does not have router functionality
# compiled in. As a result, it is not necessary to configure the
# routerrole on an MTD. At the same time, an MTD may or may not be sleepy.
#
# Use FTD by default
OPENTHREAD_TYPE ?= ftd

USEMODULE += openthread-$(OPENTHREAD_TYPE)

# Comment the following line in order to disable the OpenThread CLI
USEMODULE += openthread-cli-$(OPENTHREAD_TYPE)

USEMODULE += netdev_default

#Define PANID, CHANNEL and NETWORK_KEY used by default
OPENTHREAD_PANID ?= 0xbeef
OPENTHREAD_CHANNEL ?= 26
OPENTHREAD_NETWORK_KEY ?= "\"00:11:22:33:44:55:66:77:88:99:aa:bb:cc:dd:ee:ff\""

CFLAGS += -DOPENTHREAD_PANID=$(OPENTHREAD_PANID)
CFLAGS += -DOPENTHREAD_CHANNEL=$(OPENTHREAD_CHANNEL)
CFLAGS += -DOPENTHREAD_NETWORK_KEY=$(OPENTHREAD_NETWORK_KEY)

# Setting network name, mesh prefix and the extended PAN ID is optional
# OPENTHREAD_NETWORK_NAME ?= "\"ot-net\""
# OPENTHREAD_MESH_PREFIX ?= "\"fd:05:77:bd:d2:c1:da:be\""
# OPENTHREAD_EXT_PANID ?= "\"de:ad:00:be:ef:00:ca:fe\""

# CFLAGS += -DOPENTHREAD_NETWORK_NAME=$(OPENTHREAD_NETWORK_NAME)
# CFLAGS += -DOPENTHREAD_MESH_PREFIX=$(OPENTHREAD_MESH_PREFIX)
# CFLAGS += -DOPENTHREAD_EXT_PANID=$(OPENTHREAD_EXT_PANID)


ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox iotlab-a8-m3,$(BOARD)))
  DRIVER := at86rf231
endif
ifneq (,$(filter frdm-kw41z openlabs-kw41z-mini-256kib openlabs-kw41z-mini phynode-kw41z usb-kw41z,$(BOARD)))
  DRIVER := kw41zrf
endif

USEMODULE += $(DRIVER)

USEMODULE += random
USEMODULE += ps

# Stacksize should be increased in case 'ot debug' is used from the shell
# Else Shell will prompt > > > infinitly
CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048

#required for C++ compiling
CXXEXFLAGS += -fno-rtti
USEMODULE += cpp11-compat

include $(RIOTBASE)/Makefile.include
