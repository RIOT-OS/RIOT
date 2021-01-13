PKG_NAME=cmsis-dsp
PKG_URL=https://github.com/ARM-software/CMSIS_5
PKG_VERSION=5.4.0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

# include makefiles utils tools
include $(RIOTMAKE)/utils/strings.mk

CMSIS_DSP_MODULES =                \
    cmsis-dsp_BasicMathFunctions   \
    cmsis-dsp_CommonTables         \
    cmsis-dsp_ComplexMathFunctions \
    cmsis-dsp_ControllerFunctions  \
    cmsis-dsp_FastMathFunctions    \
    cmsis-dsp_FilteringFunctions   \
    cmsis-dsp_MatrixFunctions      \
    cmsis-dsp_StatisticsFunctions  \
    cmsis-dsp_SupportFunctions     \
    cmsis-dsp_TransformFunctions   \

.PHONY: cmsis-dsp_%

CFLAGS += -Wno-cast-align

all: $(CMSIS_DSP_MODULES)

cmsis-dsp_%:
	$(MAKE) -C $(PKG_SOURCE_DIR)/CMSIS/DSP/Source/$* -f $(CURDIR)/Makefile.cmsis-dsp MODULE=$(call lowercase,$@)
