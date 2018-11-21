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
  $(warning  * 'auto' or 'auto-ssh' to try auto-detecting the node from your experiment
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

# Number of the node to take from the list in 'auto' and 'auto-ssh' mode
# Default to 1 so the first one
IOTLAB_NODE_AUTO_NUM ?= 1

# board-archi mapping
IOTLAB_ARCHI_arduino-zero   = arduino-zero:xbee
IOTLAB_ARCHI_b-l072z-lrwan1 = st-lrwan1:sx1276
IOTLAB_ARCHI_b-l475e-iot01a = st-iotnode:multi
IOTLAB_ARCHI_iotlab-a8-m3   = a8:at86rf231
IOTLAB_ARCHI_iotlab-m3      = m3:at86rf231
IOTLAB_ARCHI_microbit       = microbit:ble
IOTLAB_ARCHI_nrf52dk        = nrf52dk:ble
IOTLAB_ARCHI_nrf52840dk     = nrf52840dk:multi
IOTLAB_ARCHI_samr21-xpro    = samr21:at86rf233
IOTLAB_ARCHI_samr30-xpro    = samr30:at86rf212b
IOTLAB_ARCHI_wsn430-v1_3b   = wsn430:cc1101
IOTLAB_ARCHI_wsn430-v1_4    = wsn430:cc2420
IOTLAB_ARCHI := $(IOTLAB_ARCHI_$(BOARD))

# Try detecting the node automatically
#  * It takes the first working node that match BOARD
#    * Check correctly deployed nodes with 'deploymentresults == "0"'
#    * Select nodes by architucture using the board-archi mapping
#    * Nodes for current server in 'auto'
ifneq (,$(filter auto auto-ssh,$(IOTLAB_NODE)))
  ifeq (,$(IOTLAB_ARCHI))
    $(error Could not find 'archi' for $(BOARD), update mapping in $(lastword $(MAKEFILE_LIST)))
  endif

  _NODES_DEPLOYED = $(shell iotlab-experiment --jmespath='deploymentresults."0"' --format='" ".join' get $(_IOTLAB_EXP_ID) --print)
  ifeq (auto,$(IOTLAB_NODE))
    _NODES_DEPLOYED := $(filter %.$(shell hostname).iot-lab.info, $(_NODES_DEPLOYED))
  endif
  _NODES_FOR_BOARD = $(shell iotlab-experiment --jmespath="items[?archi=='$(IOTLAB_ARCHI)'].network_address" --format='" ".join' get $(_IOTLAB_EXP_ID) --resources)

  _IOTLAB_NODE := $(word $(IOTLAB_NODE_AUTO_NUM),$(filter $(_NODES_DEPLOYED),$(_NODES_FOR_BOARD)))
  ifeq (auto,$(IOTLAB_NODE))
    override IOTLAB_NODE := $(firstword $(subst ., ,$(_IOTLAB_NODE)))
  else
    override IOTLAB_NODE := $(_IOTLAB_NODE)
  endif

  ifeq (,$(IOTLAB_NODE))
    $(error Could not automatically find a node for BOARD=$(BOARD))
  endif
  override IOTLAB_NODE := $(patsubst node-%,%,$(IOTLAB_NODE))
endif


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


# Display value of IOTLAB_NODE, useful to get the value calculated when using
# IOTLAB_NODE=auto or auto-ssh
.PHONY: info-iotlab-node
info-iotlab-node:
	@echo $(IOTLAB_NODE)


# Configure FLASHER, RESET, TERMPROG depending on BOARD and if on frontend

# Command to check if 'stdin' is 0. Cannot use 'cmp - <(echo 0)' without bash shell
_STDIN_EQ_0 = grep 0

ifneq (iotlab-a8-m3,$(BOARD))

  # M3 and wsn430 nodes
  FLASHER     = iotlab-node
  RESET       = iotlab-node
  _NODE_FMT   = --jmespath='keys(@)[0]' --format='int'
  FFLAGS      = $(_NODE_FMT) $(_IOTLAB_EXP_ID) $(_IOTLAB_NODELIST) --update $(IOTLAB_FLASHFILE) | $(_STDIN_EQ_0)
  RESET_FLAGS = $(_NODE_FMT) $(_IOTLAB_EXP_ID) $(_IOTLAB_NODELIST) --reset | $(_STDIN_EQ_0)

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
  _NODE_FMT   = --jmespath='keys(values(@)[0])[0]' --fmt='int'
  FFLAGS      = $(_NODE_FMT) $(_IOTLAB_EXP_ID) flash-m3 $(_IOTLAB_NODELIST) $(IOTLAB_FLASHFILE) | $(_STDIN_EQ_0)
  RESET_FLAGS = $(_NODE_FMT) $(_IOTLAB_EXP_ID) reset-m3 $(_IOTLAB_NODELIST) | $(_STDIN_EQ_0)

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
