# Define the module that is built:
MODULE = cpu

# Add a list of subdirectories, that should also be built:
DIRS = periph $(RIOTCPU)/cortexm_common

# must be set for the simple link ROM definitions to be available
CFLAGS += -DTARGET_IS_CC3200

include $(RIOTBASE)/Makefile.base
