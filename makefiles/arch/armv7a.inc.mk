# Target triple for the build. Use arm-none-eabi if you are unsure.
export TARGET_ARCH ?= arm-none-eabi

ifeq ($(MCPU),cortex-a9)
export CPU_ARCH = armv7-a
endif

# Define build specific options
export CFLAGS_CPU     = -mcpu=$(MCPU) -march=$(CPU_ARCH)
export CFLAGS_CPU    += -mfpu=neon -ffast-math -mfloat-abi=softfp -mno-unaligned-access
export CFLAGS_CPU    += -std=gnu99 --specs=nosys.specs

export CFLAGS_LINK    = -ffunction-sections -fdata-sections -fno-builtin -fshort-enums
export CFLAGS_DBG    ?= -ggdb -g3

# Configure for test driver kw2xrf
ifneq (,$(filter kw2xrf,$(USEMODULE)))
export CFLAGS_OPT    ?= -Os
else
export CFLAGS_OPT    ?= -O0
endif

export CFLAGS        += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)
export ASFLAGS       += $(CFLAGS_CPU) $(CFLAGS_DBG)

export LINKFLAGS     += -L$(RIOTCPU)/armv7a_common/ldscripts
export LINKFLAGS     += -L$(RIOTCPU)/$(CPU)/ldscripts
export LINKER_SCRIPT ?= armv7a.ld
export LINKFLAGS     += -T$(LINKER_SCRIPT) -Wl,--fatal-warnings
export LINKFLAGS     += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) -static -lgcc -nostartfiles
export LINKFLAGS     += -Wl,--gc-sections

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

# Configure for unit test fib
ifneq (,$(filter fib,$(USEMODULE)))
export CFLAGS += -DFIB_DEVEL_HELPER
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
