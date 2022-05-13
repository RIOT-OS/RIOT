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
# For a CI transition period, it is tested first.

_TRIPLES_TO_TEST := \
    riscv-none-embed \
    riscv32-none-elf \
    riscv32-unknown-elf \
    riscv32-elf \
    riscv-none-elf \
    riscv-unknown-elf \
    riscv-elf \
    riscv64-none-elf \
    riscv64-unknown-elf \
    riscv64-elf

TARGET_ARCH_RISCV ?= \
  $(strip \
    $(subst -gcc,,\
      $(notdir \
        $(word 1,\
          $(foreach triple,$(_TRIPLES_TO_TEST),$(shell which $(triple)-gcc 2> /dev/null))))))

TARGET_ARCH ?= $(TARGET_ARCH_RISCV)

ifeq (,$(TARGET_ARCH))
  $(error No RISC-V toolchain detected. Make sure a RISC-V toolchain is installed.)
endif

ifeq ($(TOOLCHAIN),gnu)
  GCC_DEFAULTS_TO_NEW_RISCV_ISA ?= $(shell echo "typedef int dont_be_pedantic;" | $(TARGET_ARCH)-gcc -march=rv32imac -mabi=ilp32 -misa-spec=2.2 -E - > /dev/null 2>&1 && echo 1 || echo 0)
endif

GCC_DEFAULTS_TO_NEW_RISCV_ISA ?= 0

CFLAGS_CPU := -march=rv32imac -mabi=ilp32

# Since RISC-V ISA specifications 20191213 instructions previously included in
# rv32imac have been moved to the ZICSR extension. See
# https://riscv.org/wp-content/uploads/2019/12/riscv-spec-20191213.pdf
#
# Select the oldest ISA spec in which ZICSR was not yet split out into an
# extension
ifeq (1,$(GCC_DEFAULTS_TO_NEW_RISCV_ISA))
  CFLAGS_CPU += -misa-spec=2.2
endif

ifeq ($(TOOLCHAIN),llvm)
  # Always use riscv32-none-elf as target triple for clang, as some
  # autodetected gcc target triples are incompatible with clang
  TARGET_ARCH_LLVM := riscv32-none-elf
else
  CFLAGS_CPU += -mcmodel=medlow -msmall-data-limit=8
  ifneq (,$(shell $(TARGET_ARCH)-gcc --help=target | grep '\-malign-data='))
    CFLAGS_CPU += -malign-data=natural
  endif
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

# Platform triple as used by Rust
RUST_TARGET = riscv32imac-unknown-none-elf
# Workaround for https://github.com/rust-lang/rust-bindgen/issues/1555
CARGO_EXTRACFLAGS += --target=riscv32
