MODULE = cpu

DIRS += periph

ifneq (,$(filter native_vfs,$(USEMODULE)))
  DIRS += vfs
endif

ifneq (,$(filter netdev_tap,$(USEMODULE)))
  DIRS += netdev_tap
endif

ifneq (,$(filter socket_zep,$(USEMODULE)))
  DIRS += socket_zep
endif

ifneq (,$(filter stdio_native,$(USEMODULE)))
  DIRS += stdio_native
endif

ifneq (,$(filter mtd_native,$(USEMODULE)))
  DIRS += mtd
endif

ifneq (,$(filter fs_native,$(USEMODULE)))
  DIRS += fs
endif

ifneq (,$(filter backtrace,$(USEMODULE)))
  DIRS += backtrace
endif

ifneq (,$(filter native_cli_eui_provider,$(USEMODULE)))
  DIRS += cli_eui_provider
endif

include $(RIOTBASE)/Makefile.base

INCLUDES = $(NATIVEINCLUDES)
