# Ensure minimal size by default
DEVELHELP ?= 0

include ../Makefile.tests_common

USEPKG += tflite-micro

# TensorFlow-Lite crashes on M4/M7 CPUs when FPU is enabled, so disable it by
# default for now
DISABLE_MODULE += cortexm_fpu
USEMODULE += mnist
EXTERNAL_MODULE_DIRS += external_modules

include $(RIOTBASE)/Makefile.include
