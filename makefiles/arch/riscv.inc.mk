# Target architecture for the build.
TARGET_ARCH_RISCV ?= riscv-none-elf

# If TARGET_ARCH wasn't set by user, also check if riscv64-unknown-elf
# or riscv-none-embed is present.
ifeq (riscv-none-elf,$(TARGET_ARCH_RISCV))
  ifeq (,$(shell which $(TARGET_ARCH_RISCV)-gcc))
    ifneq (,$(shell which riscv64-unknown-elf-gcc))
      TARGET_ARCH_RISCV := riscv64-unknown-elf
    else ifneq (,$(shell which riscv-none-embed-gcc))
      $(info Falling back to legacy riscv-none-embed toolchain)
      TARGET_ARCH_RISCV := riscv-none-embed
    endif
  endif
endif

TARGET_ARCH ?= $(TARGET_ARCH_RISCV)

# define build specific options
CFLAGS_CPU   = -march=rv32imac -mabi=ilp32 -mcmodel=medlow -msmall-data-limit=8
CFLAGS_LINK  = -nostartfiles -ffunction-sections -fdata-sections
CFLAGS_DBG  ?= -g3
CFLAGS_OPT  ?= -Os

LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts
LINKER_SCRIPT ?= $(CPU_MODEL).ld
LINKFLAGS += -T$(LINKER_SCRIPT)

CFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) $(CFLAGS_LINK)
ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
# export linker flags
LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT) -Wl,--gc-sections -static -lgcc
