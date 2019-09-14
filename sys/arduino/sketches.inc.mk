# Compile together the Arduino sketches of the application
# They are declared a as new module $(SKETCH_MODULE) in $(SKETCH_MODULE_DIR)

ifndef SKETCH_MODULE
  $(error SKETCH_MODULE undefined. It should be defined to the sketches module name)
endif
ifndef SKETCH_MODULE_DIR
  $(error SKETCH_MODULE_DIR undefined. It should be defined to the sketches module directory)
endif
ifndef SKETCHES
  $(error SKETCHES undefined. It should be defined to the list of sketches files)
endif

SNIPDIR         = $(RIOTBASE)/sys/arduino
SKETCHES_ALL    = $(SNIPDIR)/pre.snip $(SKETCHES) $(SNIPDIR)/post.snip
SKETCH_CPP     ?= arduino_sketches.cpp
SKETCH_GENERATED_FILES = $(SKETCH_MODULE_DIR)/Makefile $(SKETCH_MODULE_DIR)/$(SKETCH_CPP)

# Building the module files
#   Do not use $^ in receipes as Makefile is also a prerequisite
$(SKETCH_MODULE_DIR)/Makefile: $(SKETCH_MODULE_DIR)/$(SKETCH_CPP)
	$(Q)echo 'SRCXX = $(SKETCH_CPP)'               > $@
	$(Q)echo 'include $$(RIOTBASE)/Makefile.base' >> $@
$(SKETCH_MODULE_DIR)/$(SKETCH_CPP): $(SKETCHES_ALL)
	@mkdir -p $(@D)
	$(Q)cat $(SKETCHES_ALL) > $@

# Make everything rebuild if current makefile changes
_ARDUINO_SKETCHES_MAKEFILE := $(lastword $(MAKEFILE_LIST))
$(SKETCH_MODULE_DIR)/$(SKETCH_CPP): $(_ARDUINO_SKETCHES_MAKEFILE)
$(SKETCH_MODULE_DIR)/Makefile: $(_ARDUINO_SKETCHES_MAKEFILE)

# HACK Rebuild cpp files every time in case one of the `SKETCHES` is deleted
$(SKETCH_MODULE_DIR)/$(SKETCH_CPP): FORCE
