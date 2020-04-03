ifeq (,$(CPU_MODEL))
  $(error CPU_MODEL must have been defined by the board/cpu Makefile.features)
endif

# Target triple for the build. Use arm-none-eabi if you are unsure.
TARGET_ARCH ?= arm-none-eabi

# define build specific options
CFLAGS_CPU   = -mcpu=$(MCPU) -mlittle-endian -mthumb $(CFLAGS_FPU)

ifneq (llvm,$(TOOLCHAIN))
  # Clang (observed with v3.7) does not understand  -mno-thumb-interwork, only add if
  # not building with LLVM
  CFLAGS += -mno-thumb-interwork

  # work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85606
  ifneq (,$(filter cortex-m0%,$(CPU_ARCH)))
    CFLAGS_CPU += -march=armv6s-m
  endif
endif

CFLAGS_LINK  = -ffunction-sections -fdata-sections -fno-builtin -fshort-enums
CFLAGS_DBG  ?= -ggdb -g3
CFLAGS_OPT  ?= -Os

CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)

ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts -L$(RIOTCPU)/cortexm_common/ldscripts
LINKER_SCRIPT ?= $(CPU_MODEL).ld
LINKFLAGS += -T$(LINKER_SCRIPT) -Wl,--fatal-warnings

LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) -static -lgcc -nostartfiles
LINKFLAGS += -Wl,--gc-sections

# extract version inside the first parentheses
ARM_GCC_VERSION = $(shell $(TARGET_ARCH)-gcc --version | sed -n '1 s/[^(]*(\([^\)]*\)).*/\1/p')

# Ubuntu bionic gcc-arm-none-eabi compiler is not supported
# Both when using gnu and llvm toolchains
#
#     /usr/bin/arm-none-eabi-gcc --version | head -n 1
#     arm-none-eabi-gcc (15:6.3.1+svn253039-1build1) 6.3.1 20170620
#
ARM_GCC_UNSUPPORTED += 15:6.3.1%  # ubuntu bionic, ignore 'svn' build part

# Should not raise an error on the host system version when building in docker
ifneq (1,$(BUILD_IN_DOCKER))
  ifneq (,$(filter $(ARM_GCC_UNSUPPORTED),$(ARM_GCC_VERSION)))
    $(warning $(TARGET_ARCH)-gcc version not supported)
    $(warning $(shell $(TARGET_ARCH)-gcc --version | head -n 1))
    $(warning The currently recommanded version is the one installed in the riotdocker image)
    $(warning https://github.com/RIOT-OS/riotdocker/blob/master/Dockerfile)
    ifeq (1,$(WERROR))
      $(error This check can be ignored by building with 'WERROR=0')
    endif # WERROR
  endif # ARM_GCC_UNSUPPORTED
endif # BUILD_IN_DOCKER

CFLAGS += -DCPU_MODEL_$(call uppercase_and_underscore,$(CPU_MODEL))
CFLAGS += -DCPU_ARCH_$(call uppercase_and_underscore,$(CPU_ARCH))

# Add corresponding FPU CFLAGS
# clang assumes there is an FPU, no CFLAGS necessary
ifneq (llvm, $(TOOLCHAIN))
  ifeq ($(CPU_ARCH),cortex-m7)
    _CORTEX_HW_FPU_CFLAGS = -mfloat-abi=hard -mfpu=fpv5-sp-d16
  else
    _CORTEX_HW_FPU_CFLAGS = -mfloat-abi=hard -mfpu=fpv4-sp-d16
  endif
endif
# Add soft or hard FPU CFLAGS depending on the module
# NOTE: This can be turned into normal conditional syntax once #9913 is fixed
CFLAGS_FPU ?= $(if $(filter cortexm_fpu,$(USEMODULE)),$(_CORTEX_HW_FPU_CFLAGS),-mfloat-abi=soft)

ifeq ($(CPU_ARCH),cortex-m4f)
  MCPU = cortex-m4
else
  MCPU ?= $(CPU_ARCH)
endif

# CMSIS DSP needs to know about the CPU core
ifneq (,$(filter cmsis-dsp,$(USEPKG)))
  # definition needed to use cmsis-dsp headers
  ifeq ($(CPU_ARCH),cortex-m0)
    CFLAGS += -DARM_MATH_CM0
  else ifeq ($(CPU_ARCH),cortex-m0plus)
    CFLAGS += -DARM_MATH_CM0PLUS
  else ifeq ($(CPU_ARCH),cortex-m3)
    CFLAGS += -DARM_MATH_CM3
  else ifeq ($(CPU_ARCH),cortex-m4)
    CFLAGS += -DARM_MATH_CM4
  else ifeq ($(CPU_ARCH),cortex-m4f)
    CFLAGS += -DARM_MATH_CM4
  else ifeq ($(CPU_ARCH),cortex-m7)
    CFLAGS += -DARM_MATH_CM7
  else ifeq ($(CPU_ARCH),cortex-m23)
    CFLAGS += -DARM_MATH_CM23
  endif
endif

# Explicitly tell the linker to link the startup code.
#   Without this the interrupt vectors will not be linked correctly!
VECTORS_O ?= $(BINDIR)/cpu/vectors.o
ifeq ($(COMMON_STARTUP),)
  UNDEF += $(VECTORS_O)
endif

# CPU depends on the cortex-m common module, so include it:
include $(RIOTCPU)/cortexm_common/Makefile.include

# Avoid overriding the default rule:
all:

# Rule to generate assembly listings from ELF files:
%.lst: %.elf
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@
