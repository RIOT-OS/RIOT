PKG_NAME=wamr
PKG_URL=https://github.com/bytecodealliance/wasm-micro-runtime.git
PKG_VERSION=main
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

#less Wall TODO: get things fixed upstream
CFLAGS :=  $(filter-out -pedantic, $(CFLAGS))
CFLAGS += -Wno-format
CFLAGS += -Wno-strict-prototypes
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-cast-function-type

#Prepapre includes for cmake
RIOT_INCLUDES = $(filter-out -%,$(subst -I,,$(INCLUDES)))

#translate (CPU_ARCH) to Build Target
#WAMR_BUILD_TARGET is "X86_32" "AARCH64[sub]", "ARM[sub]",
#                     "THUMB[sub]", "MIPS" or "XTENSA"
#no msp430, no AVR support for now

ifeq ($(CPU),native)
#$(CPU) is defined for every CPU
#everyone build on x86_32
  WAMR_BUILD_TARGET = X86_32
else ifeq ($(findstring arm,$(CPU_ARCH)),arm)
  WAMR_BUILD_TARGET = THUMB
else ifeq ($(CPU_ARCH),mips32r2)
  WAMR_BUILD_TARGET = MIPS
else ifeq ($(CPU_ARCH),xtensa)
  WAMR_BUILD_TARGET = XTENSA
endif

ifeq ($(QUIET),0)
  CMAKEMAKEFLAGS += VERBOSE=1
endif

#WAMR_CONFIG will be included into the cmake
ifneq ($(WAMR_CONFIG),)
  WAMR_CMAKE_FLAGS += "-DWAMR_CONFIG=$(WAMR_CONFIG)"
endif

WAMR_CMAKE_FLAGS += "-DRIOT_INCLUDES=$(RIOT_INCLUDES)"\
                     -DWAMR_BUILD_TARGET=$(WAMR_BUILD_TARGET)\
                     -DWAMR_ROOT_DIR=$(PKG_SOURCE_DIR)/ \
                     -DCMAKE_SYSTEM_NAME=Generic \
                     -DCMAKE_SYSTEM_PROCESSOR="$(MCPU)" \
                     -DCMAKE_C_COMPILER=$(CC) \
                     -DCMAKE_C_COMPILER_WORKS=TRUE \


all: $(BINDIR)/libwamr.a

$(BINDIR)/libwamr.a: $(PKG_BUILD_DIR)/libwamr.a
	cp $< $@

$(PKG_BUILD_DIR)/libwamr.a: $(PKG_BUILD_DIR)/Makefile
	+$(MAKE) -C $(PKG_BUILD_DIR) $(CMAKEMAKEFLAGS)

$(PKG_BUILD_DIR)/Makefile: $(PKG_PREPARED) print_build_target
	ASMFLAGS="${CFLAGS}" cmake -B$(PKG_BUILD_DIR) $(WAMR_CMAKE_FLAGS)

print_build_target:
	@echo CPU_ARCH: $(CPU_ARCH)
	@echo CPU: $(CPU)
	@echo CFLAGS: $(CFLAGS)
	@echo WAMR_BUILD_TARGET: $(WAMR_BUILD_TARGET)
	@echo WAMR_CONFIG: $(WAMR_CONFIG)
