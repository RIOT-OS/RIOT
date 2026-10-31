include ../Makefile.pkg_common

USEPKG += bplib
USEMODULE += bplib_stor_vfs
# No CLA is used here

USEMODULE += vfs_default
USEMODULE += fmt
USEMODULE += shell
USEMODULE += shell_cmd_bplib
USEMODULE += shell_cmd_ps
USEMODULE += shell_cmd_vfs

# More than 1 channel and contact is not needed here
CFLAGS += -DBPLIB_MAX_NUM_CONTACTS=1
CFLAGS += -DBPLIB_MAX_NUM_CHANNELS=1

# This is the interval at which the storage is searched. This is quite expensive
# but for this test it should better be fast to prevent long waits.
CFLAGS += -DCONFIG_BPLIB_MAINTENANCE_INTERVAL=2000
# The storage limit is set that only one bundle can ever be stored
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=4096

CFLAGS += -DFS_NATIVE_DIR=\"$(CURDIR)/native_fs\"
CFLAGS += -DSTDIO_RX_BUFSIZE=256
CFLAGS += -DFS_NATIVE_DIR=\"$(CURDIR)/native_fs\"

include $(RIOTBASE)/Makefile.include
