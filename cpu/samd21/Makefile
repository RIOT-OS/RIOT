# define the module that is build
MODULE = cpu

MODULE_DEFS_NEEDED += \
  periph_dma \
  #

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/cortexm_common $(RIOTCPU)/sam0_common vectors

include $(RIOTBASE)/Makefile.base
