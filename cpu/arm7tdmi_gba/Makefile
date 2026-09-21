MODULE = cpu

include $(RIOTCPU)/$(CPU)/Makefile.include

DISABLE_MODULE += core_thread

DIRS = $(RIOTCPU)/arm7_common periph

ifneq (,$(filter stdio_fb,$(USEMODULE)))
  DIRS += stdio_fb
endif

include $(RIOTBASE)/Makefile.base
