include ../Makefile.tests_common

USEPKG += utensor

# Include input data as blob
BLOBS += digit

# Include C++ model and training weights code as an external module
USEMODULE += models
EXTERNAL_MODULE_DIRS += external_modules

include $(RIOTBASE)/Makefile.include

# The application requires a little more stacksize on ARM when building with
# LLVM
ifneq (,$(filter arch_arm,$(FEATURES_USED)))
  ifeq (llvm,$(TOOLCHAIN))
    CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048
  endif
endif
