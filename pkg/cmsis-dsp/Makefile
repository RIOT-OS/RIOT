PKG_NAME=cmsis-dsp
PKG_URL=https://github.com/ARM-software/CMSIS_5
PKG_VERSION=5.4.0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CMSIS_DSP_MODULES =                \
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

.PHONY: cmsis-dsp_%

CFLAGS += -Wno-cast-align

all: $(CMSIS_DSP_MODULES)

cmsis-dsp_%:
	$(MAKE) -C $(PKG_SOURCE_DIR)/CMSIS/DSP/Source/$(DIR_$*) -f $(CURDIR)/Makefile.cmsis-dsp MODULE=$@
