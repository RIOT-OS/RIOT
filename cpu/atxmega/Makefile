# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph $(RIOTCPU)/avr8_common/

# needed for 'chars_from' below, as 'Makefile.base' is only included at the end
include $(RIOTMAKE)/utils/variables.mk
include $(RIOTMAKE)/utils/strings.mk

ifneq (,$(findstring a1,$(call chars_from,8,$(CPU_MODEL))))
    DIRS += ebi
endif

include $(RIOTBASE)/Makefile.base
