# name of your application
APPLICATION = bplib_cla_udp

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

CFLAGS += -DBPLIB_QM_TX_QUEUE_DEPTH=8
CFLAGS += -DBPLIB_MAX_NUM_CONTACTS=1
CFLAGS += -DBPLIB_MAX_NUM_CHANNELS=1
CFLAGS += -DBPLIB_LOCAL_EID_NODE_NUM=200
CFLAGS += -DBPLIB_STOR_MOUNTPOINT=\"/nvm0/\"
CFLAGS += -DBPLIB_STOR_DIRNAME=\"bp\"
CFLAGS += -DBPLIB_TIME_FILE_NAME=\"/nvm0/bp_time.dat\"

USEMODULE += shell
USEMODULE += shell_cmd_ps

SRC += main.c

USEPKG += bplib
USEMODULE += bplib_default_fwp
USEMODULE += bplib_stor_vfs
USEMODULE += bplib_cla_udp

USEMODULE += netdev_tap
USEMODULE += shell_cmd_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += auto_init_gnrc_netif

USEMODULE += vfs_default
CFLAGS += -DFS_NATIVE_DIR=\"$(CURDIR)/native\"

include $(RIOTBASE)/Makefile.include
