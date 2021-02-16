MODULE = cc26xx_cc13xx

DIRS = periph
ifneq (,$(filter saul_cc26xx_cc13xx_batmon,$(USEMODULE)))
  DIRS += batmon
endif

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

include $(RIOTBASE)/Makefile.base
