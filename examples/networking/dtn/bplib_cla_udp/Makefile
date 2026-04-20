# name of your application
APPLICATION = bplib_cla_udp

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../..

# Include bplib and the configuration modules
USEPKG += bplib
USEMODULE += bplib_stor_vfs_ordered     # Ordered/Hierarchical storage
USEMODULE += bplib_cla_udp              # UDPCL

# The number of contacts that bplib uses / the number of tables reserved.
# For this example there is only one, the remote client or server. When a node
# should forward between two other nodes, this should be increased to the required
# number of concurrent connections
CFLAGS += -DBPLIB_MAX_NUM_CONTACTS=1
# The number of channels / channel tables. Here bundles are only sent to one EID
# and received at one service number concurrently.
CFLAGS += -DBPLIB_MAX_NUM_CHANNELS=1
# Configure the path at which bundles will be saved by the storage implementation.
# Depending on the storage implementation, subfolders may be created here.
CFLAGS += -DCONFIG_BPLIB_STOR_BASE=\"/nvm0/bp\"
# Configure the path at which the time file of bplib will be saved. This keeps track
# of DTN times of previous boots. When the DTN time is not known, this time-keeping
# ability of bplib can be disabled with the `bplib_no_vfs` module.
CFLAGS += -DBPLIB_TIME_FILE_NAME=\"/nvm0/bp_time.dat\"

# Example configuration
ifeq (,$(REMOTE))
  $(warning REMOTE IPv6 address not set, defaulting to [::1])
  REMOTE := ::1
endif
CFLAGS += -DBPLIB_EXAMPLE_IP_REMOTE=\"$(REMOTE)\"
ifeq (,$(REMOTE_EID_NODE))
  $(warning REMOTE_EID_NODE destination node number not set, defaulting to 100)
  REMOTE_EID_NODE := 100
endif
# This sets the destination node number
CFLAGS += -DBPLIB_EXAMPLE_REMOTE_NODE_NO=$(REMOTE_EID_NODE)
ifeq (,$(LOCAL_EID_NODE))
  $(warning LOCAL_EID_NODE destination node number not set, defaulting to 100)
  LOCAL_EID_NODE := 100
endif
# This sets the node number of the device that runs bplib.
CFLAGS += -DBPLIB_LOCAL_EID_NODE_NUM=$(LOCAL_EID_NODE)

USEMODULE += shell
USEMODULE += shell_cmd_ps
USEMODULE += shell_cmd_gnrc_netif

# Netif for UDPCL
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_sock_udp

USEMODULE += vfs_default
# For the 3 device scenario, bind the storage to the local ID, otherwise multiple
# nodes will share the storage on native, which is not the point of communicating
# over BP
CFLAGS += -DFS_NATIVE_DIR=\"$(CURDIR)/native$(LOCAL_EID_NODE)\"

include $(RIOTBASE)/Makefile.include
