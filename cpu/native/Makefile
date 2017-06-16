MODULE = cpu

DIRS += periph
DIRS += vfs

ifeq ($(shell uname -s),Darwin)
  CFLAGS += -D_XOPEN_SOURCE=600 -D_DARWIN_C_SOURCE
endif

ifneq (,$(filter netdev_tap,$(USEMODULE)))
  DIRS += netdev_tap
endif

ifneq (,$(filter socket_zep,$(USEMODULE)))
  DIRS += socket_zep
endif

ifneq (,$(filter mtd_native,$(USEMODULE)))
  DIRS += mtd
endif

ifneq (,$(filter can_linux,$(USEMODULE)))
  DIRS += can
endif
ifneq (,$(filter trace,$(USEMODULE)))
	DIRS += trace
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
