# name of your application
APPLICATION = minimal
include ../Makefile.tests_common

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

#
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

#
DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
