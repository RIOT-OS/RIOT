# define the module that is build (not strictly necessary)
MODULE = atmega_common

# add a list of subdirectories, that should also be build
DIRS = periph avr_libc_extra

ifneq (,$(filter cpu_atmega_common_cxx,$(USEMODULE)))
  DIRS += cxx
endif

include $(RIOTBASE)/Makefile.base
