PKG_NAME=cmsis
PKG_URL=https://github.com/ARM-software/CMSIS_5
PKG_VERSION=2b7495b8535bdcb306dac29b9ded4cfb679d7e5c  # 5.9.0
PKG_LICENSE=Apache-2.0

CMSIS_NN_MODULES =                 \
  cmsis-nn_activationfunctions     \
  cmsis-nn_convolutionfunctions    \
  cmsis-nn_fullyconnectedfunctions \
  cmsis-nn_nnsupportfunctions      \
  cmsis-nn_poolingfunctions        \
  cmsis-nn_softmaxfunctions        \
  #

CMSIS_DSP_MODULES =              \
  cmsis-dsp_basicmathfunctions   \
  cmsis-dsp_commontables         \
  cmsis-dsp_complexmathfunctions \
  cmsis-dsp_controllerfunctions  \
  cmsis-dsp_fastmathfunctions    \
  cmsis-dsp_filteringfunctions   \
  cmsis-dsp_matrixfunctions      \
  cmsis-dsp_statisticsfunctions  \
  cmsis-dsp_supportfunctions     \
  cmsis-dsp_transformfunctions   \
  #

CMSIS_DSP_MODULES_USED = $(filter $(CMSIS_DSP_MODULES),$(USEMODULE))
CMSIS_NN_MODULES_USED = $(filter $(CMSIS_NN_MODULES),$(USEMODULE))
CMSIS_MODULES_USED = $(CMSIS_DSP_MODULES_USED) $(CMSIS_NN_MODULES_USED)

PKG_SPARSE_PATHS=CMSIS/Core/Include

ifneq (, $(CMSIS_NN_MODULES_USED))
  PKG_SPARSE_PATHS+=CMSIS/NN
  CMSIS_DSP_NEEDED=1
endif
ifneq (, $(CMSIS_DSP_MODULES_USED)$(CMSIS_DSP_NEEDED))
  PKG_SPARSE_PATHS+=CMSIS/DSP
endif


include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

DIR_activationfunctions        := ActivationFunctions
DIR_convolutionfunctions       := ConvolutionFunctions
DIR_fullyconnectedfunctions    := FullyConnectedFunctions
DIR_nnsupportfunctions         := NNSupportFunctions
DIR_poolingfunctions           := PoolingFunctions
DIR_softmaxfunctions           := SoftmaxFunctions

DIR_basicmathfunctions      := BasicMathFunctions
DIR_commontables            := CommonTables
DIR_complexmathfunctions    := ComplexMathFunctions
DIR_controllerfunctions     := ControllerFunctions
DIR_fastmathfunctions       := FastMathFunctions
DIR_filteringfunctions      := FilteringFunctions
DIR_matrixfunctions         := MatrixFunctions
DIR_statisticsfunctions     := StatisticsFunctions
DIR_supportfunctions        := SupportFunctions
DIR_transformfunctions      := TransformFunctions

.PHONY: cmsis-dsp_% cmsis-nn_%

all: $(CMSIS_MODULES_USED)

cmsis-dsp_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/CMSIS/DSP/Source/$(DIR_$*) -f $(CURDIR)/Makefile.cmsis-dsp MODULE=$@

cmsis-nn_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/CMSIS/NN/Source/$(DIR_$*) -f $(CURDIR)/Makefile.cmsis-nn MODULE=$@
