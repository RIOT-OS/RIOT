# name of your application
APPLICATION = bplib_cla_ble

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52840dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

CFLAGS += -DBPLIB_QM_TX_QUEUE_DEPTH=8
CFLAGS += -DBPLIB_MAX_NUM_CONTACTS=1
CFLAGS += -DBPLIB_MAX_NUM_CHANNELS=1
CFLAGS += -DBPLIB_LOCAL_EID_NODE_NUM=100

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

SRC += main.c

USEPKG += bplib
USEMODULE += bplib_stor_void    # No storage, void bundles that can't be used directly
USEMODULE += bplib_cla_ble      # Use the BLE CLA
USEMODULE += bplib_no_vfs       # Remove bplib's internal time saving in a file
# Due to a) void storage and b) bplib_no_vfs no vfs is needed.

include $(RIOTBASE)/Makefile.include
