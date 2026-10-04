include ../Makefile.pkg_common

USEPKG += bplib
USEMODULE += bplib_stor_vfs_ordered
# No CLA is used here

USEMODULE += vfs_default
USEMODULE += core_thread_flags
USEMODULE += fmt
USEMODULE += shell
USEMODULE += shell_cmd_ps
USEMODULE += shell_cmd_bplib
USEMODULE += shell_cmd_vfs

# More than 1 channel and contact is not needed here
CFLAGS += -DBPLIB_MAX_NUM_CONTACTS=1
CFLAGS += -DBPLIB_MAX_NUM_CHANNELS=1
CFLAGS += -DFS_NATIVE_DIR=\"$(CURDIR)/native_fs\"

include $(RIOTBASE)/Makefile.include
