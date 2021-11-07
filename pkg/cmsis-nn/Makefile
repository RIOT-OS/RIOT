PKG_NAME=cmsis-nn
PKG_URL=https://github.com/ARM-software/CMSIS_5
PKG_VERSION=5.6.0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

CMSIS_NN_MODULES =                   \
    cmsis-nn_ActivationFunctions     \
    cmsis-nn_ConvolutionFunctions    \
    cmsis-nn_FullyConnectedFunctions \
    cmsis-nn_NNSupportFunctions      \
    cmsis-nn_PoolingFunctions        \
    cmsis-nn_SoftmaxFunctions        \

.PHONY: cmsis-nn_%

all: $(CMSIS_NN_MODULES)

cmsis-nn_%:
	$(MAKE) -C $(PKG_SOURCE_DIR)/CMSIS/NN/Source/$* -f $(CURDIR)/Makefile.cmsis-nn MODULE=$@
