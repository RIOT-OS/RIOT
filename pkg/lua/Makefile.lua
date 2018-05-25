SRC := $(filter-out lua.c luac.c,$(wildcard *.c))

# This builds for native using POSIX system calls and some extra libraries, and
# removes a compiler warning that warns against using tmpnam().
ifeq ($(BOARD),native)
  CFLAGS += -DLUA_USE_LINUX
endif

include $(RIOTBASE)/Makefile.base
