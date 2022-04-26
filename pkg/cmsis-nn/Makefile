PKG_NAME=cmsis-nn
PKG_URL=https://github.com/ARM-software/CMSIS_5
PKG_VERSION=5.6.0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

CMSIS_NN_MODULES =                   \
    cmsis-nn_activationfunctions     \
    cmsis-nn_convolutionfunctions    \
    cmsis-nn_fullyconnectedfunctions \
    cmsis-nn_nnsupportfunctions      \
    cmsis-nn_poolingfunctions        \
    cmsis-nn_softmaxfunctions        \
    #

DIR_activationfunctions        := ActivationFunctions
DIR_convolutionfunctions       := ConvolutionFunctions
DIR_fullyconnectedfunctions    := FullyConnectedFunctions
DIR_nnsupportfunctions         := NNSupportFunctions
DIR_poolingfunctions           := PoolingFunctions
DIR_softmaxfunctions           := SoftmaxFunctions

.PHONY: cmsis-nn_%

all: $(CMSIS_NN_MODULES)

cmsis-nn_%:
	$(MAKE) -C $(PKG_SOURCE_DIR)/CMSIS/NN/Source/$(DIR_$*) -f $(CURDIR)/Makefile.cmsis-nn MODULE=$@
