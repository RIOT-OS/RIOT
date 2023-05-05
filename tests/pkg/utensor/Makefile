include ../Makefile.pkg_common

USEPKG += utensor

# Include input data as blob
BLOBS += digit

# Include C++ model and training weights code as an external module
USEMODULE += models
EXTERNAL_MODULE_DIRS += external_modules

include $(RIOTBASE)/Makefile.include

CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE
