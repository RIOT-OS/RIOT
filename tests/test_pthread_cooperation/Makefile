export PROJECT = test_pthread_cooperation
include ../Makefile.tests_common

USEMODULE += posix
USEMODULE += pthread

ifeq ($(BOARD),native)
    CFLAGS += -isystem $(RIOTBASE)/sys/posix/pthread/include
else
    export INCLUDES = -I$(RIOTBASE)/sys/posix/pthread/include \
                      -I$(RIOTBASE)/sys/posix/include
endif

include $(RIOTBASE)/Makefile.include
