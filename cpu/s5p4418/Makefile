# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = bsp periph $(RIOTCPU)/armv7a_common

INCLUDES += -I$(RIOTBASE)/cpu/s5p4418/include/
INCLUDES += -I$(RIOTBASE)/cpu/s5p4418/include/bsp/
INCLUDES += -I$(RIOTBASE)/cpu/s5p4418/include/system/

include $(RIOTBASE)/Makefile.base
