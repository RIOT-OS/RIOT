# Only locate non-pseudo-modules
MODS2LOCATE := $(filter-out $(PSEUDOMODULES) $(NO_AUTOLOCATE),\
               $(USEMODULE)) $(filter $(NO_PSEUDOMODULES),$(USEMODULE))
EXTERNAL_MODULE_PATHS := $(sort $(foreach dir,$(EXTERNAL_MODULE_DIRS),\
$(foreach mod,$(MODS2LOCATE),$(dir $(wildcard $(dir)/$(mod)/Makefile)))))

# Ignore modules already located as external modules
MODS2LOCATE := $(filter-out $(basename $(EXTERNAL_MODULE_PATHS)),$(MODS2LOCATE))

MODULE_PATHS := $(sort $(foreach dir,$(MODULE_DIRS),\
                $(foreach mod,$(MODS2LOCATE),$(dir $(wildcard $(dir)/$(mod)/Makefile)))))
