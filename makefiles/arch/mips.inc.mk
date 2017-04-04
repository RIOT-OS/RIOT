ifndef MIPS_ELF_ROOT
ifneq ($(BUILD_IN_DOCKER),1) #Don't error when BUILD_IN_DOCKER=1 as it _is_ set in DOCKER
    $(error "Please set $$(MIPS_ELF_ROOT) and ensure $$(MIPS_ELF_ROOT)/bin is on your PATH")
endif
endif

# Target triple for the build.
export TARGET_ARCH ?= mips-mti-elf

export ABI=32

ifneq ($(BUILD_IN_DOCKER),1) #Don't error when BUILD_IN_DOCKER=1 as MIPS_ELF_ROOT _is_ set in DOCKER
include $(MIPS_ELF_ROOT)/share/mips/rules/mipshal.mk
endif

# define build specific options
export CFLAGS_CPU   = -EL -std=gnu99
export CFLAGS_LINK  = -ffunction-sections -fno-builtin -fshort-enums #-fdata-sections
export CFLAGS_DBG = -O0 -g2
export CFLAGS_OPT = -Os -g2

export CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_OPT)
#$(CFLAGS_DBG)

ifeq ($(USE_HARD_FLOAT),1)
    export CFLAGS += -mhard-float
else
    export CFLAGS += -msoft-float #hard-float is the default so we must set soft-float
    export LINKFLAGS += -msoft-float
endif

ifeq ($(USE_DSP),1)
    export CFLAGS += -mdsp
endif

export ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_OPT) #$(CFLAGS_DBG)

export LINKFLAGS += $(MIPS_HAL_LDFLAGS) -mabi=$(ABI)
export LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT)
export LINKFLAGS += -Wl,--gc-sections

# This CPU implementation is using the new core/CPU interface:
export CFLAGS += -DCOREIF_NG=1
