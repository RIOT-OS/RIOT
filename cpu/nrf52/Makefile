# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/cortexm_common $(RIOTCPU)/nrf5x_common

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

# build the nrfusb USB peripheral driver if required
ifneq (,$(filter nrfusb,$(USEMODULE)))
  DIRS += usb
endif

include $(RIOTBASE)/Makefile.base
