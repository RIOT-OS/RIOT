include ../Makefile.pkg_common

# required packages
USEPKG += uzlib

# This application requires a large stacksize for the main thread
ifeq (,$(filter native%,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048
endif

include $(RIOTBASE)/Makefile.include
