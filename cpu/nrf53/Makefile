# define the module that is build
MODULE = cpu

MODULE_DEFS_NEEDED += \
  periph_rtt \
  #

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/cortexm_common $(RIOTCPU)/nrf5x_common vectors

include $(RIOTBASE)/Makefile.base
