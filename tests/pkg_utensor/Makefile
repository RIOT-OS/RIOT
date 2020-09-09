include ../Makefile.tests_common

USEPKG += utensor

# Include input data as blob
BLOBS += digit

# Include C++ model and training weights code as an external module
USEMODULE += models
EXTERNAL_MODULE_DIRS += $(CURDIR)/models

include $(RIOTBASE)/Makefile.include
