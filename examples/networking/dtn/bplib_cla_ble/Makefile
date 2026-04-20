# name of your application
APPLICATION = bplib_cla_ble

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52840dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include bplib and the configuration modules
USEPKG += bplib
USEMODULE += bplib_stor_void    # No storage, void bundles that can't be used directly
USEMODULE += bplib_cla_ble      # Use the BLE CLA
USEMODULE += bplib_no_vfs       # Remove bplib's internal time saving in a file
# Due to a) void storage and b) bplib_no_vfs no vfs is needed.

# The number of contacts that bplib uses / the number of tables reserved.
# For this example there is only one, the remote client or server. In a generic
# scenario, for example when a node translates between two CL protocols, e.g.
# can receive BLE and then forwards on UDP, this has to be increased.
CFLAGS += -DBPLIB_MAX_NUM_CONTACTS=1
# The number of channels / channel tables. Here bundles are only sent to one EID
# and received at one service number concurrently.
CFLAGS += -DBPLIB_MAX_NUM_CHANNELS=1

# Example configuration
ROLE ?= server

ifeq (,$(filter client server,$(ROLE)))
  $(error Unknown ROLE for example '$(ROLE)'. Should be 'client' or 'server'.)
  ROLE := server
endif

ifneq (,$(filter client,$(ROLE)))
  ifeq (,$(REMOTE))
    $(warning REMOTE MAC address not set, will not connect anywhere, compiling anyways)
  endif
  CFLAGS += -DBPLIB_EXAMPLE_ROLE_CLIENT=1
  CFLAGS += -DBPLIB_EXAMPLE_REMOTE=\"$(REMOTE)\"

  # The node number of the local EID / remote EID. Both devices need to have these reversed
  CFLAGS += -DBPLIB_EXAMPLE_REMOTE_NODE_NO=200
  CFLAGS += -DBPLIB_LOCAL_EID_NODE_NUM=100
else
  CFLAGS += -DBPLIB_EXAMPLE_ROLE_CLIENT=0
  CFLAGS += -DBPLIB_EXAMPLE_REMOTE_NODE_NO=100
  CFLAGS += -DBPLIB_LOCAL_EID_NODE_NUM=200
endif

USEMODULE += shell
USEMODULE += shell_cmd_ps

CFLAGS += -DCONFIG_NIMBLE_AUTOADV_DEVICE_NAME='"DTN_ble-cla"'
CFLAGS += -DCONFIG_NIMBLE_AUTOADV_START_MANUALLY=1

CFLAGS += -DMYNEWT_VAL_BLE_MAX_CONNECTIONS=1
CFLAGS += -DMYNEWT_VAL_BLE_L2CAP_COC_MAX_NUM=1

MSYS_CNT ?= 40
CFLAGS += -DMYNEWT_VAL_BLE_LL_CFG_FEAT_DATA_LEN_EXT=1
CFLAGS += -DMYNEWT_VAL_BLE_LL_MAX_PKT_SIZE=251
CFLAGS += -DMYNEWT_VAL_BLE_MAX_CONNECTIONS=1
CFLAGS += -DMYNEWT_VAL_BLE_L2CAP_COC_MAX_NUM=1
CFLAGS += -DMYNEWT_VAL_BLE_L2CAP_COC_MPS=MYNEWT_VAL_BLE_LL_MAX_PKT_SIZE
CFLAGS += -DMYNEWT_VAL_MSYS_1_BLOCK_COUNT=$(MSYS_CNT)
CFLAGS += -DMYNEWT_VAL_MSYS_1_BLOCK_SIZE="(MYNEWT_VAL_BLE_L2CAP_COC_MPS + 48)"

include $(RIOTBASE)/Makefile.include
