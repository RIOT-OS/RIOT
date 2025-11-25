MODULE = cpu

include $(RIOTCPU)/$(CPU)/Makefile.include

DIRS = $(RIOTCPU)/arm7_common

ifneq (,$(filter stdio_%,$(USEMODULE)))
  DIRS += stdio_fb
endif

include $(RIOTBASE)/Makefile.base
