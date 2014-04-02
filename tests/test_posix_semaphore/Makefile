export PROJECT = test_posix_semaphore
include ../Makefile.tests_common

USEMODULE += posix 

export INCLUDES += -I$(RIOTBASE)/sys/posix/include
DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
