# Target triple for the build. Use arm-none-eabi if you are unsure.
export TARGET_ARCH ?= arm-none-eabi

# define build specific options
export CFLAGS_CPU   = -mcpu=$(MCPU) -mlittle-endian -mthumb $(CFLAGS_FPU)

ifneq (llvm,$(TOOLCHAIN))
# Clang (observed with v3.7) does not understand  -mno-thumb-interwork, only add if
# not building with LLVM
export CFLAGS      += -mno-thumb-interwork

# work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85606
ifneq (,$(filter cortex-m0%,$(CPU_ARCH)))
  CFLAGS_CPU += -march=armv6s-m
endif
endif

export CFLAGS_LINK  = -ffunction-sections -fdata-sections -fno-builtin -fshort-enums
export CFLAGS_DBG  ?= -ggdb -g3
export CFLAGS_OPT  ?= -Os

export CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)

export ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
export LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts -L$(RIOTCPU)/cortexm_common/ldscripts
export LINKER_SCRIPT ?= $(CPU_MODEL).ld
export LINKFLAGS += -T$(LINKER_SCRIPT) -Wl,--fatal-warnings

export LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) -static -lgcc -nostartfiles
export LINKFLAGS += -Wl,--gc-sections

# Tell the build system that the CPU depends on the Cortex-M common files:
export USEMODULE += cortexm_common
# Export the peripheral drivers to be linked into the final binary:
export USEMODULE += periph
# include common periph code
export USEMODULE += periph_common
export USEMODULE += cortexm_common_periph

# all cortex MCU's use newlib as libc
export USEMODULE += newlib

# set default for CPU_MODEL
export CPU_MODEL ?= $(CPU)

# Temporary LLVM/Clang Workaround:
# report cortex-m0 instead of cortex-m0plus if llvm/clang (<= 3.6.2) is used
# llvm/clang version 3.6.2 still does not support the cortex-m0plus mcpu type
ifeq (llvm,$(TOOLCHAIN))
ifeq (cortex-m0plus,$(CPU_ARCH))
LLVM_UNSUP = $(shell clang -target arm-none-eabi -mcpu=$(CPU_ARCH) -c -x c /dev/null -o /dev/null \
                     > /dev/null 2>&1 || echo 1 )
ifeq (1,$(LLVM_UNSUP))
CPU_ARCH = cortex-m0
endif
endif
endif

# export the CPU model and architecture
MODEL = $(shell echo $(CPU_MODEL) | tr 'a-z' 'A-Z')
export CFLAGS += -DCPU_MODEL_$(MODEL)
ARCH = $(shell echo $(CPU_ARCH) | tr 'a-z-' 'A-Z_')
export CFLAGS += -DCPU_ARCH_$(ARCH)

# set the compiler specific CPU and FPU options
ifeq ($(CPU_ARCH),cortex-m4f)
# TODO: enable hard floating points for the M4F once the context save/restore
#       code is adjusted to take care of FPU registers
#export CFLAGS_FPU += -mfloat-abi=hard -mfpu=fpv4-sp-d16
export MCPU := cortex-m4
endif
CFLAGS_FPU ?= -mfloat-abi=soft
export MCPU ?= $(CPU_ARCH)

# CMSIS DSP needs to know about the CPU core
ifneq (,$(filter cmsis-dsp,$(USEPKG)))
# definition needed to use cmsis-dsp headers
ifeq ($(CPU_ARCH),cortex-m0)
export CFLAGS += -DARM_MATH_CM0
else ifeq ($(CPU_ARCH),cortex-m0plus)
export CFLAGS += -DARM_MATH_CM0PLUS
else ifeq ($(CPU_ARCH),cortex-m3)
export CFLAGS += -DARM_MATH_CM3
else ifeq ($(CPU_ARCH),cortex-m4)
export CFLAGS += -DARM_MATH_CM4
else ifeq ($(CPU_ARCH),cortex-m4f)
export CFLAGS += -DARM_MATH_CM4
else ifeq ($(CPU_ARCH),cortex-m7)
export CFLAGS += -DARM_MATH_CM7
endif
endif

# Explicitly tell the linker to link the startup code.
#   Without this the interrupt vectors will not be linked correctly!
VECTORS_O ?= $(BINDIR)/cpu/vectors.o
ifeq ($(COMMON_STARTUP),)
export UNDEF += $(VECTORS_O)
endif

# CPU depends on the cortex-m common module, so include it:
include $(RIOTCPU)/cortexm_common/Makefile.include

# use the nano-specs of Newlib when available
USEMODULE += newlib_nano
export USE_NANO_SPECS = 1

# Avoid overriding the default rule:
all:

# Rule to generate assembly listings from ELF files:
%.lst: %.elf
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@
