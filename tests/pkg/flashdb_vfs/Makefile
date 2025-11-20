include ../Makefile.pkg_common

# select the VFS backend
USEMODULE += flashdb_vfs
# enable time series database
USEMODULE += flashdb_tsdb
# enable key-value database
USEMODULE += flashdb_kvdb

# the test makes use of time()
USEMODULE += libc_gettimeofday

USEMODULE += vfs_default
USEMODULE += vfs_auto_format

CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE*2

include $(RIOTBASE)/Makefile.include
