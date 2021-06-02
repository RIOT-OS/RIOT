# Use a single iot-lab node with RIOT
# ===================================
#
# Supported:
#  * flash
#  * reset
#  * term
#
# Tested on m3/a8-m3/samr21/arduino-zero nodes
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
  $(warning  * 'auto' to try auto-detecting the node from your experiment
  $(error)
endif

ifeq (auto-ssh,$(IOTLAB_NODE))
  $(info $(COLOR_YELLOW)IOTLAB_NODE=auto-ssh is deprecated and will be removed after \
         2010.07 is released, use IOTLAB_NODE=auto instead$(COLOR_RESET))
  override IOTLAB_NODE := auto
endif

IOTLAB_AUTH ?= $(HOME)/.iotlabrc
IOTLAB_USER ?= $(shell cut -f1 -d: $(IOTLAB_AUTH))

ifneq (0,$(shell command -v iotlab-experiment -h 2>&1 > /dev/null ; echo $$?))
  $(info $(COLOR_RED)'iotlab-experiment' command is not available \
	        please consider installing it from \
	        https://pypi.python.org/pypi/iotlabcli$(COLOR_RESET))
  $(error )
endif

ifeq (iotlab-a8-m3,$(BOARD))
  ifneq (,$(filter flash% reset,$(MAKECMDGOALS)))
    ifneq (0,$(shell command -v iotlab-ssh -h 2>&1 > /dev/null ; echo $$?))
      $(info $(COLOR_RED)'iotlab-ssh' command is not available \
	          please consider installing it from \
	          https://pypi.python.org/pypi/iotlabsshcli$(COLOR_RESET))
      $(error )
    endif
  endif
endif

# Optional Experiment id. Required when having multiple experiments
IOTLAB_EXP_ID ?=

# Specify experiment-id option if provided
_IOTLAB_EXP_ID := $(if $(IOTLAB_EXP_ID),--id $(IOTLAB_EXP_ID))

# Number of the node to take from the list in 'auto' mode
# Default to 1 so the first one
IOTLAB_NODE_AUTO_NUM ?= 1

# There are several deprecated and incompatible features used here that were
# introduced between versions 2 and 3 of the IoT-LAB cli tools.
# For backward compatibility, we manage these changes here.
_CLI_TOOLS_MAJOR_VERSION ?= $(shell iotlab-experiment --version | cut -f1 -d.)
ifeq (2,$(_CLI_TOOLS_MAJOR_VERSION))
  _NODES_DEPLOYED = $(shell iotlab-experiment --jmespath='deploymentresults."0"' --format='" ".join' get $(_IOTLAB_EXP_ID) --print)
  _NODES_LIST_OPTION = --resources
  _NODES_FLASH_OPTION = --update
else
  _NODES_DEPLOYED = $(shell iotlab-experiment --jmespath='"0"' --format='" ".join' get $(_IOTLAB_EXP_ID) --deployment)
  _NODES_LIST_OPTION = --nodes
  _NODES_FLASH_OPTION = --flash
  ifeq (,$(filter firefly zigduino,$(BOARD)))
    # All boards in IoT-LAB except firefly can be flashed using $(BINFILE).
    # On IoT-LAB, firefly only accepts $(ELFFILE) and WSN320 boards on accept $(HEXFILE).
    # Using $(BINFILE) speeds up the firmware upload since the file is much
    # smaller than an elffile.
    # This feature is only available in cli-tools version >= 3.
    FLASHFILE = $(BINFILE)
  endif
endif

# Try detecting the node automatically
#  * It takes the first working node that match BOARD
#    * Check correctly deployed nodes with 'deploymentresults == "0"'
#    * Select nodes by architucture using the board-archi mapping
#    * Nodes for current server in 'auto'
ifeq (auto,$(IOTLAB_NODE))
  ifeq (,$(IOTLAB_ARCHI))
    $(error Could not find 'archi' for $(BOARD), update mapping in $(lastword $(MAKEFILE_LIST)))
  endif

  ifneq (,$(IOT_LAB_FRONTEND_FQDN))
    _NODES_DEPLOYED := $(filter %.$(IOT_LAB_FRONTEND_FQDN), $(_NODES_DEPLOYED))
  endif
  _NODES_FOR_BOARD = $(shell iotlab-experiment --jmespath="items[?archi=='$(IOTLAB_ARCHI)'].network_address" --format='" ".join' get $(_IOTLAB_EXP_ID) $(_NODES_LIST_OPTION))

  _IOTLAB_NODE := $(word $(IOTLAB_NODE_AUTO_NUM),$(filter $(_NODES_DEPLOYED),$(_NODES_FOR_BOARD)))
  ifneq (,$(IOT_LAB_FRONTEND_FQDN))
    override IOTLAB_NODE := $(firstword $(subst ., ,$(_IOTLAB_NODE)))
  else
    override IOTLAB_NODE := $(_IOTLAB_NODE)
  endif

  ifeq (,$(IOTLAB_NODE))
    $(error Could not automatically find a node for BOARD=$(BOARD))
  endif
endif

# Handle IOTLAB_NODE specified with comma separated form
# saclay,m3,1 => m3-1.saclay.iot-lab.info)
IOTLAB_NODE_COMMA := $(subst $(comma), ,$(IOTLAB_NODE))
ifneq ($(IOTLAB_NODE_COMMA),$(IOTLAB_NODE))
  _IOTLAB_NODE_TYPE := $(word 2, $(IOTLAB_NODE_COMMA))
  _IOTLAB_NODE_ID := $(firstword $(subst +, ,$(subst -, ,$(word 3, $(IOTLAB_NODE_COMMA)))))
  _IOTLAB_SITE := $(word 1, $(IOTLAB_NODE_COMMA))
  override IOTLAB_NODE := $(_IOTLAB_NODE_TYPE)-$(_IOTLAB_NODE_ID).$(_IOTLAB_SITE).iot-lab.info

  # If launched from an IoT-LAB frontend, check that the frontends are matching
  ifneq (,$(IOT_LAB_FRONTEND_FQDN))
    ifneq ($(IOT_LAB_FRONTEND_FQDN),$(_IOTLAB_SITE).iot-lab.info)
      $(error $(COLOR_RED)IoT-LAB Frontend doesn't match ($(IOT_LAB_FRONTEND_FQDN) != $(_IOTLAB_SITE).iot-lab.info)$(COLOR_RESET))
    endif
  endif
endif

# Work with node url without 'node-'
override IOTLAB_NODE := $(patsubst node-%,%,$(IOTLAB_NODE))

# Create node list and optionally frontend url
ifeq (,$(IOT_LAB_FRONTEND_FQDN))
  # Running from a local computer
  # m3-380.grenoble.iot-lab.info    -> grenoble,m3,380
  # a8-1.grenoble.iot-lab.info      -> grenoble,a8,1
  _NODELIST_SED := 's/\([^.]*\)-\([^.]*\).\([^.]*\).*/\3,\1,\2/'
  _IOTLAB_NODELIST := --list $(shell echo '$(IOTLAB_NODE)' | sed $(_NODELIST_SED))

  # Remove the node type-number part
  _IOTLAB_SERVER := $(shell echo '$(IOTLAB_NODE)' | sed 's/[^.]*.//')
  _IOTLAB_AUTHORITY = $(IOTLAB_USER)@$(_IOTLAB_SERVER)
else
  # Running from an IoT-LAB frontend
  # m3-380    -> $(hostname),m3,380
  # a8-1      -> $(hostname),a8,1
  _NODELIST_SED := 's/\([^.]*\)-\([^.]*\)/$(shell hostname),\1,\2/'
  _IOTLAB_NODELIST := --list $(shell echo '$(IOTLAB_NODE)' | sed $(_NODELIST_SED))
endif


# Display value of IOTLAB_NODE, useful to get the value calculated when using
# IOTLAB_NODE=auto
.PHONY: info-iotlab-node
info-iotlab-node:
	@echo $(IOTLAB_NODE)


# Configure FLASHER, RESET, TERMPROG depending on BOARD and if on frontend

ifneq (iotlab-a8-m3,$(BOARD))

  # M3 nodes
  FLASHER     = iotlab-node
  RESET       = iotlab-node
  _NODE_FMT   = --jmespath='keys(@)[0]' --format='lambda ret: exit(int(ret))'
  FFLAGS      = $(_NODE_FMT) $(_IOTLAB_EXP_ID) $(_IOTLAB_NODELIST) $(_NODES_FLASH_OPTION) $(FLASHFILE)
  RESET_FLAGS = $(_NODE_FMT) $(_IOTLAB_EXP_ID) $(_IOTLAB_NODELIST) --reset

  ifeq (,$(IOT_LAB_FRONTEND_FQDN))
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
  FFLAGS      = $(_NODE_FMT) $(_IOTLAB_EXP_ID) flash $(_IOTLAB_NODELIST) $(FLASHFILE)
  RESET_FLAGS = $(_NODE_FMT) $(_IOTLAB_EXP_ID) reset $(_IOTLAB_NODELIST)

  TERMPROG  = ssh
  ifeq (,$(IOT_LAB_FRONTEND_FQDN))
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
