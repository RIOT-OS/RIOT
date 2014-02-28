PROJECT = test_pthread_cleanup
include ../Makefile.tests_common

USEMODULE += pthread

ifeq ($(BOARD),native)
	CFLAGS += -isystem $(RIOTBASE)/sys/posix/pthread/include
else
	INCLUDES += -I$(RIOTBASE)/sys/posix/pthread/include
endif

include $(RIOTBASE)/Makefile.include
