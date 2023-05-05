include ../Makefile.pkg_common

# select the MTD backend
USEMODULE += flashdb_mtd
# enable time series database
USEMODULE += flashdb_tsdb
# enable key-value database
USEMODULE += flashdb_kvdb

# the test makes use of time()
USEMODULE += libc_gettimeofday

CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE

CFLAGS += -DFAL_PART0_LABEL=\"/fdb_kvdb1\"
CFLAGS += -DFAL_PART0_LENGTH=8*1024

CFLAGS += -DFAL_PART1_LABEL=\"/fdb_tsdb1\"
CFLAGS += -DFAL_PART1_LENGTH=8*1024

include $(RIOTBASE)/Makefile.include
