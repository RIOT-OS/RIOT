# Target architecture for the build.

# The correct triple for 32 bit embedded RISC-V targets is "riscv32-none-elf".
# However, is unknown is used as sys (in place of none), it will also fall back
# to none. Finally, if the GCC RISC-V cross compiler is compiled with full
# multilib support, it can create both 32 bit and 64 bit binaries. Often this is
# indicated with "riscv" being used instead of "riscv32/riscv64", but e.g.
# Ubuntu uses "riscv64-unknown-elf" despite being able to produce both 32 and
# 64 bit binaries. We'll test all possible combinations from the most correct
# triple to the least correct triple all that might be able to produce our
# binaries. Finally, "riscv-none-embed" is also tested for compatibility with
# an previously popular legacy toolchain.

_TRIPLES_TO_TEST := \
    riscv32-none-elf \
    riscv-none-elf \
    riscv32-unknown-elf \
    riscv-unknown-elf \
    riscv64-none-elf \
    riscv64-unknown-elf \
    riscv-none-embed

TARGET_ARCH_RISCV ?= \
  $(strip \
    $(subst -gcc,,\
      $(notdir \
        $(word 1,\
          $(foreach triple,$(_TRIPLES_TO_TEST),$(shell which $(triple)-gcc 2> /dev/null))))))

TARGET_ARCH ?= $(TARGET_ARCH_RISCV)

# define build specific options
CFLAGS_CPU   = -march=rv32imac -mabi=ilp32
ifeq ($(TOOLCHAIN),llvm)
  # Always use riscv32-none-elf as target triple for clang, as some
  # autodetected gcc target triples are incompatible with clang
  TARGET_ARCH_LLVM := riscv32-none-elf
else
  CFLAGS_CPU += -mcmodel=medlow -msmall-data-limit=8
endif
CFLAGS_LINK  = -ffunction-sections -fdata-sections
CFLAGS_DBG  ?= -g3
CFLAGS_OPT  ?= -Os

LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts -L$(RIOTCPU)/riscv_common/ldscripts
LINKER_SCRIPT ?= $(CPU_MODEL).ld
LINKFLAGS += -T$(LINKER_SCRIPT)

CFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) $(CFLAGS_LINK)
ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
# export linker flags
LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT) -nostartfiles -Wl,--gc-sections -static -lgcc
