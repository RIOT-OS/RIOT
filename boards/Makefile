OLD_NONRECURSIVE := $(NONRECURSIVE)
NONRECURSIVE = 1
# First include for current module (should be done before any 'include')
include $(RIOTBASE)/Makefile.base

# Then take care of other modules

include Makefile.modules

MAKEFILE_DIRS = $(sort $(foreach module, $(USEMODULE), $(MODULE_DIR_$(module))))
include $(MAKEFILE_DIRS:%=%/Makefile)
NONRECURSIVE := $(OLD_NONRECURSIVE)
