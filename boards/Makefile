OLD_NONRECURSIVE := $(NONRECURSIVE)
NONRECURSIVE = 1

include Makefile.modules

MAKEFILE_DIRS = $(sort $(foreach module, $(USEMODULE), $(MODULE_DIR_$(module))))
include $(MAKEFILE_DIRS:%=%/Makefile)
NONRECURSIVE := $(OLD_NONRECURSIVE)
