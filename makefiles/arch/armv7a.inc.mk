# Target triple for the build. Use arm-none-eabi if you are unsure.
export TARGET_ARCH ?= arm-none-eabi

export CPU_ARCH = cortex-a9
export MCPU = cortex-a9
export CFLAGS_FPU = -mfloat-abi=soft
export CPU_MODEL ?= $(CPU)

# Define build specific options
export CFLAGS_CPU   = -mcpu=cortex-a9 -march=armv7-a -mfpu=neon -ffast-math -mfloat-abi=softfp -mno-unaligned-access
export CFLAGS_DBG   = -g -ggdb -Wall --specs=nosys.specs 

export CFLAGS       = $(CFLAGS_CPU) $(CFLAGS_DBG) -std=gnu99 -O0
export ASFLAGS      = -mcpu=cortex-a9 -march=armv7-a 

export LINKFLAGS = $(CFLAGS_CPU) $(CFLAGS_DBG)
export LINKER_SCRIPT ?= armv7a.ld
export LINKFLAGS += -L$(RIOTCPU)/armv7a_common/ldscripts
export LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts
export LINKFLAGS += -T$(LINKER_SCRIPT) -static -lgcc -nostartfiles -Wl,--fatal-warnings

# This CPU implementation is using the new core/CPU interface:
export CFLAGS += -DCOREIF_NG=1

export USEMODULE += armv7a_common
export USEMODULE += periph
export USEMODULE += periph_common
export USEMODULE += newlib

MODEL = $(shell echo $(CPU_MODEL) | tr 'a-z' 'A-Z')
export CFLAGS += -DCPU_MODEL_$(MODEL)
ARCH = $(shell echo $(CPU_ARCH) | tr 'a-z-' 'A-Z_')
export CFLAGS += -DCPU_ARCH_$(ARCH)

# CMSIS DSP needs to know about the CPU core
ifneq (,$(filter cmsis-dsp,$(USEPKG)))
# definition needed to use cmsis-dsp headers
ifeq ($(CPU_ARCH),cortex-a9)
export CFLAGS += -DARM_MATH_CORTEX_A9
endif
endif

# Explicitly tell the linker to link the startup code.
#   Without this the interrupt vectors will not be linked correctly!
ifeq ($(COMMON_STARTUP),)
export UNDEF += $(BINDIR)/startup/start.o
endif

# CPU depends on the armv7a common module, so include it:
include $(RIOTCPU)/armv7a_common/Makefile.include

# use the nano-specs of Newlib when available
USEMODULE += newlib_nano
export USE_NANO_SPECS = 1

# Avoid overriding the default rule:
all:

# Rule to generate assembly listings from ELF files:
%.lst: %.elf
	$(OBJDUMP) $(OBJDUMPFLAGS) $< > $@
