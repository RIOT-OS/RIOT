# Target architecture for the build.
export TARGET_ARCH ?= riscv-none-embed

# define build specific options
CFLAGS_CPU   = -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8
CFLAGS_LINK  = -nostartfiles -ffunction-sections -fdata-sections
CFLAGS_DBG  ?= -g3
CFLAGS_OPT  ?= -Os

export LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts
export LINKER_SCRIPT ?= $(CPU_MODEL).ld
export LINKFLAGS += -T$(LINKER_SCRIPT)

CFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) $(CFLAGS_LINK)
ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
# export linker flags
export LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT) -Wl,--gc-sections -static -lgcc
