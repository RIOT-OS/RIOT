BASE_MODULE := prng
SUBMODULES := 1

include Makefile.modules

DIRS += $(foreach module, $(USEMODULE), $(MODULE_DIR_$(module)))

include $(RIOTBASE)/Makefile.base
