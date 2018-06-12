include ../Makefile.tests_common

USEMODULE += vfs
USEMODULE += xtimer
USEMODULE += printf_float

FS ?= littlefs

USEMODULE += $(FS)

ifeq ($(FS),littlefs)
    # Set vfs file and dir buffer sizes
    CFLAGS += -DVFS_FILE_BUFFER_SIZE=52 -DVFS_DIR_BUFFER_SIZE=44
    # Reduce LFS_NAME_MAX to 31 (as VFS_NAME_MAX default)
    CFLAGS += -DLFS_NAME_MAX=31
endif

include $(RIOTBASE)/Makefile.include
