APPLICATION = bootloader
BOARD ?= samr21-xpro

USEMODULE += bootloader
USEMODULE += firmware

#CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE
#DISABLE_MODULE += auto_init
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(5*THREAD_STACKSIZE_DEFAULT\)
# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/..

include $(RIOTBASE)/Makefile.include
