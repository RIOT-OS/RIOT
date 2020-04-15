MODULE ?= periph

# enable submodules
SUBMODULES := 1

# in case of e.g., <cpu>_periph_common, use periph_* as submodule prefix
BASE_MODULE := periph

include $(RIOTBASE)/Makefile.base
