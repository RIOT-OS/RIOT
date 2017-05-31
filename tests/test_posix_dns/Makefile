export PROJECT = test_posix_time
include ../Makefile.tests_common

USEMODULE += posix

ifeq ($(BOARD),native)
	CFLAGS += -isystem $(RIOTBASE)/sys/posix/include
else
	INCLUDES += -I$(RIOTBASE)/sys/posix/include
endif

include $(RIOTBASE)/Makefile.include
