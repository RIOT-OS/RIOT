# Ensure minimal size by default
DEVELHELP ?= 0

include ../Makefile.tests_common

# Other available example: hello_world
EXAMPLE ?= mnist

USEPKG += tensorflow-lite

# internal mnist example is available as an external module
ifeq (mnist,$(EXAMPLE))
  # TensorFlow-Lite crashes on M4/M7 CPUs when FPU is enabled, so disable it by
  # default for now
  DISABLE_MODULE += cortexm_fpu
  USEMODULE += $(EXAMPLE)
  EXTERNAL_MODULE_DIRS += external_modules
else
  # Use upstream example
  USEMODULE += tensorflow-lite-$(EXAMPLE)
endif

include $(RIOTBASE)/Makefile.include
