APPLICATION = test_registry
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += registry
# Enable the use of int64 types on the registry
CFLAGS += -DCONFIG_REGISTRY_USE_INT64

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)

# Choose one type of storage facility:
# USE DUMMY STORAGE (RAM)
#USEMODULE += registry_store_dummy

# USE FILE STORAGE
USEMODULE += registry_store_file
USEMODULE += mtd
USEMODULE += mtd_sdcard
# FAT as file system
USEMODULE += fatfs
# Use VFS
USEMODULE += vfs
USEMODULE += fatfs_vfs
CFLAGS += -DVFS_FILE_BUFFER_SIZE=72 -DVFS_DIR_BUFFER_SIZE=44

ifneq (,$(filter registry_store_dummy,$(USEMODULE)))
  DIRS += dummy
endif

include $(RIOTBASE)/Makefile.include
