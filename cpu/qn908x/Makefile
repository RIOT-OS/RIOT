# define the module that is build
MODULE = cpu

MODULE_DEFS_NEEDED += \
  periph_wdt \
  #

# add a list of subdirectories that should also be built
DIRS = periph $(RIOTCPU)/cortexm_common vendor

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

include $(RIOTBASE)/Makefile.base
