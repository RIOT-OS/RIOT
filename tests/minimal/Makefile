DEVELHELP ?= 0
include ../Makefile.tests_common

CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

DISABLE_MODULE += auto_init

# disable newlib. safe to do on non-newlib platforms.
DISABLE_MODULE += newlib newlib_nano

include $(RIOTBASE)/Makefile.include
