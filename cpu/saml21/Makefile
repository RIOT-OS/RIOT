# define the module that is build
MODULE = cpu

MODULE_DEFS_NEEDED += \
  periph_dma \
  periph_pm \
  periph_usbdev \
  #

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/cortexm_common $(RIOTCPU)/sam0_common

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

include $(RIOTBASE)/Makefile.base
