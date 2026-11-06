include ../Makefile.tests_common

USEMODULE += fatfs_vfs
USEMODULE += fatfs_vfs_format

USEMODULE += embunit

CFLAGS += -DPAGE_SIZE=512
CFLAGS += -DPAGE_PER_SECTOR=1
CFLAGS += -DSECTOR_COUNT=4096
CFLAGS += -DFATFS_FFCONF_OPT_USE_LFN

include $(RIOTBASE)/Makefile.include
