MODULE = cpu_dir

include Makefile.modules
include $(wildcard */Makefile.modules)

DIRS = $(foreach module, $(USEMODULE), $(MODULE_DIR_$(module)))
include $(RIOTBASE)/Makefile.base
