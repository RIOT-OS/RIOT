# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/fam_stm32

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

include $(RIOTBASE)/Makefile.base
