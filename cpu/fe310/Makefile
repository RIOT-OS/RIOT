# define the module that is built
MODULE = cpu

# add a list of subdirectories, that should also be built
DIRS = periph vendor

ifneq (1,$(PICOLIBC))
  DIRS += nano
endif

include $(RIOTBASE)/Makefile.base
