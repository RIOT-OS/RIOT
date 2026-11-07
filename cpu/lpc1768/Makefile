# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/cortexm_common

# add LPC1768 specific drivers, if enabled
ifneq (,$(filter lpc1768_eth,$(USEMODULE)))
  DIRS += drivers/eth
endif

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

include $(RIOTBASE)/Makefile.base
