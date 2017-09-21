MODULE = cpu

DIRS += periph
DIRS += vfs

ifneq (,$(filter netdev_tap,$(USEMODULE)))
  DIRS += netdev_tap
endif
ifneq (,$(filter mtd_native,$(USEMODULE)))
  DIRS += mtd
endif

ifneq (,$(filter can_linux,$(USEMODULE)))
  DIRS += can
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
