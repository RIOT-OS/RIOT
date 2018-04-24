# Use a single iot-lab node with RIOT
# ===================================
#
# Supported:
#  * flash
#  * reset
#  * term
#
# Tested on m3/a8-m3/wsn430/samr21/arduino-zero nodes
#
# It can be run:
# * From your computer by setting IOTLAB_NODE to the full url like
#   m3-380.grenoble.iot-lab.info or a8-1.grenoble.iot-lab.info
# * From IoT-LAB frontend by setting IOTLAB_NODE to the short url like
#   m3-380 or a8-1
#
# # Usage
#
#   make BOARD=iotlab-m3 IOTLAB_NODE=m3-380.grenoble.iot-lab.info flash term
#
# It is the user responsibility to start an experiment beforehand
#
# If the user has multiple running experiments, the one to use must be
# configured with the `IOTLAB_EXP_ID` setting
#
# Prerequisites
# -------------
#
# * Install iotlab-cli-tools: https://github.com/iot-lab/cli-tools
# * Configure your account with 'iotlab-auth'
# * Register your computer public key in your ssh keys list on your profile:
#   https://www.iot-lab.info/tutorials/configure-your-ssh-access/
# * If using an A8 node from a frontend:
#   * Register the frontend public key in your ssh keys list on your profile.
#     https://www.iot-lab.info/tutorials/ssh-cli-client/
#
# Additional for A8 nodes
#
# * Install iotlab-ssh-cli-tools https://github.com/iot-lab/ssh-cli-tools

ifeq (,$(IOTLAB_NODE))
  $(warning IOTLAB_NODE undefined, it should be defined to:)
  $(warning  * <type>-<number>.<site>.iot-lab.info when run from your computer)
  $(warning    Example: m3-380.grenoble.iot-lab.info or a8-1.grenoble.iot-lab.info)
  $(warning  * <type>-<number> when run from iot-lab frontend)
  $(warning    Example: m3-380 or a8-1)
  $(error)
endif

IOTLAB_AUTH ?= $(HOME)/.iotlabrc
IOTLAB_USER ?= $(shell cut -f1 -d: $(IOTLAB_AUTH))

# Optional Experiment id. Required when having multiple experiments
IOTLAB_EXP_ID ?=

# File to use for flashing
IOTLAB_FLASHFILE ?= $(ELFFILE)

# Specify experiment-id option if provided
_IOTLAB_EXP_ID := $(if $(IOTLAB_EXP_ID),--id $(IOTLAB_EXP_ID))


# If the IOTLAB_NODE format is:
# * 'type-num.iot.lab.info' assume it is run from your computer
# * 'type-num' assume it is run from iot-lab frontend
ifneq (,$(filter %.iot-lab.info, $(IOTLAB_NODE)))
  _IOTLAB_ON_FRONTEND =
else
  _IOTLAB_ON_FRONTEND = 1
endif
# Work with node url without 'node-'
override IOTLAB_NODE := $(patsubst node-%,%,$(IOTLAB_NODE))


# Create node list and optionally frontend url
ifeq (,$(_IOTLAB_ON_FRONTEND))
  # m3-380.grenoble.iot-lab.info    -> grenoble,m3,380
  # a8-1.grenoble.iot-lab.info      -> grenoble,a8,1
  _NODELIST_SED := 's/\([^.]*\)-\([^.]*\).\([^.]*\).*/\3,\1,\2/'
  _IOTLAB_NODELIST := --list $(shell echo '$(IOTLAB_NODE)' | sed $(_NODELIST_SED))

  # Remove the node type-number part
  _IOTLAB_SERVER := $(shell echo '$(IOTLAB_NODE)' | sed 's/[^.]*.//')
  _IOTLAB_AUTHORITY = $(IOTLAB_USER)@$(_IOTLAB_SERVER)
else
  # m3-380    -> $(hostname),m3,380
  # a8-1      -> $(hostname),a8,1
  _NODELIST_SED := 's/\([^.]*\)-\([^.]*\)/$(shell hostname),\1,\2/'
  _IOTLAB_NODELIST := --list $(shell echo '$(IOTLAB_NODE)' | sed $(_NODELIST_SED))
endif


# Configure FLASHER, RESET, TERMPROG depending on BOARD and if on frontend
ifneq (iotlab-a8-m3,$(BOARD))

  # M3 and wsn430 nodes
  FLASHER     = iotlab-node
  RESET       = iotlab-node
  FFLAGS      = $(_IOTLAB_EXP_ID) $(_IOTLAB_NODELIST) --update $(IOTLAB_FLASHFILE)
  RESET_FLAGS = $(_IOTLAB_EXP_ID) $(_IOTLAB_NODELIST) --reset

  ifeq (,$(_IOTLAB_ON_FRONTEND))
    TERMPROG  = ssh
    TERMFLAGS = -t $(_IOTLAB_AUTHORITY) 'socat - tcp:$(IOTLAB_NODE):20000'
  else
    TERMPROG  = socat
    TERMFLAGS = - tcp:$(IOTLAB_NODE):20000
  endif

else

  # A8-M3 node
  FLASHER     = iotlab-ssh
  RESET       = iotlab-ssh
  FFLAGS      = $(_IOTLAB_EXP_ID) flash-m3 $(_IOTLAB_NODELIST) $(IOTLAB_FLASHFILE)
  RESET_FLAGS = $(_IOTLAB_EXP_ID) reset-m3 $(_IOTLAB_NODELIST)

  TERMPROG  = ssh
  ifeq (,$(_IOTLAB_ON_FRONTEND))
    # Proxy ssh through the iot-lab frontend
    TERMFLAGS = -oProxyCommand='ssh $(_IOTLAB_AUTHORITY) -W %h:%p'
  else
    # Empty existing RIOT TERMFLAGS
    TERMFLAGS =
  endif
  TERMFLAGS += -oStrictHostKeyChecking=no -t root@node-$(IOTLAB_NODE) 'socat - open:/dev/ttyA8_M3,b$(BAUD),echo=0,raw'

endif

# Debugger not supported
DEBUGGER =
DEBUGGER_FLAGS =
DEBUGSERVER =
DEBUGSERVER_FLAGS =
