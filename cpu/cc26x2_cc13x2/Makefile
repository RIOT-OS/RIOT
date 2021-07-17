# Define the module that is built:
MODULE = cpu

# Add a list of subdirectories, that should also be built:
DIRS = periph $(RIOTCPU)/cortexm_common $(RIOTCPU)/cc26xx_cc13xx

ifneq (,$(filter cc26x2_cc13x2_rf,$(USEMODULE)))
  DIRS += radio
  DIRS += rfc
endif

include $(RIOTBASE)/Makefile.base
