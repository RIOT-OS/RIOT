# Define the module that is built:
MODULE = cpu

# Add a list of subdirectories, that should also be built:
DIRS = periph $(RIOTCPU)/cortexm_common

# cc2538_rf radio driver
ifneq (,$(filter cc2538_rf,$(USEMODULE)))
  DIRS += radio
endif

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

include $(RIOTBASE)/Makefile.base
