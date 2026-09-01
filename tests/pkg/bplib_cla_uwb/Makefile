include ../Makefile.pkg_common

BOARD ?= nrf52840dk

EXTERNAL_MODULE_DIRS += $(PROJECT_BASE)/modules
EXTERNAL_BOARD_DIRS += $(PROJECT_BASE)/boards

# Include bplib and the configuration modules
USEPKG += bplib
USEMODULE += bplib_stor_void    # No storage, void bundles that can't be used directly
USEMODULE += bplib_cla_uwb      # Use the UWB CLA
USEMODULE += bplib_no_vfs       # Remove bplib's internal time saving in a file
# Due to a) void storage and b) bplib_no_vfs, no vfs is needed.

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += shell_cmd_ps

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
  # The node number of the local EID / remote EID. Both devices need to have these reversed
  CFLAGS += -DBPLIB_EXAMPLE_REMOTE_NODE_NO=200
  CFLAGS += -DBPLIB_LOCAL_EID_NODE_NUM=100
else
  CFLAGS += -DBPLIB_EXAMPLE_REMOTE_NODE_NO=100
  CFLAGS += -DBPLIB_LOCAL_EID_NODE_NUM=200
endif

include $(RIOTBASE)/Makefile.include
