MODULE = cpu

include $(RIOTCPU)/$(CPU)/Makefile.include

DIRS = $(RIOTCPU)/arm7_common

ifneq (,$(filter stdio_fb,$(USEMODULE)))
  DIRS += stdio_fb
endif

include $(RIOTBASE)/Makefile.base
