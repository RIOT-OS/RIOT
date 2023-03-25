MODULE = cpu

include $(RIOTCPU)/$(CPU)/Makefile.include

DIRS = $(RIOTCPU)/arm7_common
DIRS += graphics
DIRS += dma

ifneq (,$(filter stdio_fb,$(USEMODULE)))
  DIRS += stdio_fb
endif

include $(RIOTBASE)/Makefile.base
