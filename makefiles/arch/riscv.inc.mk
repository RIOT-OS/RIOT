# Target architecture for the build.

# The correct triple for 32 bit embedded RISC-V targets is "riscv32-none-elf".
# However, is unknown is used as sys (in place of none), it will also fall back
# to none. Finally, if the GCC RISC-V cross compiler is compiled with full
# multilib support, it can create both 32 bit and 64 bit binaries. Often this is
# indicated with "riscv" being used instead of "riscv32/riscv64", but e.g.
# Ubuntu uses "riscv64-unknown-elf" despite being able to produce both 32 and
# 64 bit binaries. We'll test all possible combinations from the most correct
# triple to the least correct triple all that might be able to produce our
# binaries.

_TRIPLES_TO_TEST := \
    riscv32-none-elf \
    riscv32-unknown-elf \
    riscv32-elf \
    riscv-none-elf \
    riscv-unknown-elf \
    riscv-elf \
    riscv64-none-elf \
    riscv64-unknown-elf \
    riscv64-elf

# Do not test at run time if building with docker: The host may have no
# RISC-V toolchain installed or a different one
ifeq (1,$(BUILD_IN_DOCKER))
  TARGET_ARCH_RISCV := riscv-none-elf
endif

TARGET_ARCH_RISCV ?= \
  $(strip \
    $(subst -gcc,,\
      $(notdir \
        $(word 1,\
          $(shell which $(addsuffix -gcc,$(_TRIPLES_TO_TEST)) 2> /dev/null)))))

TARGET_ARCH ?= $(TARGET_ARCH_RISCV)

# Convert to a simply expanded variable here, as a recursively expended
# variable would result in detecting the toolchain each and every time again the
# toolchain is referenced.
TARGET_ARCH := $(TARGET_ARCH)

ifeq (,$(TARGET_ARCH))
  $(error No RISC-V toolchain detected. Make sure a RISC-V toolchain is installed.)
endif

CFLAGS_CPU := -march=rv32imac_zicsr -mabi=ilp32

# LLVM<17 does not support Zicsr extension in ISA-string
ifeq ($(TOOLCHAIN),llvm)
  CFLAGS_CPU := -march=rv32imac -mabi=ilp32
endif

# Always use riscv32-none-elf as target triple for clang, as some
# autodetected gcc target triples are incompatible with clang
TARGET_ARCH_LLVM := riscv32-none-elf
ifneq ($(TOOLCHAIN),llvm)
  CFLAGS_CPU += -mcmodel=medlow -msmall-data-limit=8
  # We cannot invoke the compiler on the host system if build in docker.
  # Instead, hard-code the required flags for the docker toolchain here
  ifeq (1,$(BUILD_IN_DOCKER))
    CFLAGS_CPU += -malign-data=natural
  else
    ifneq (,$(shell $(TARGET_ARCH)-gcc --help=target | grep '\-malign-data='))
      CFLAGS_CPU += -malign-data=natural
    endif
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
