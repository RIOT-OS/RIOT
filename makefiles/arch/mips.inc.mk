# Target triple for the build.
TARGET_ARCH_MIPS ?= mips-mti-elf
TARGET_ARCH ?= $(TARGET_ARCH_MIPS)

ABI = 32

# Default values for the linker script symbols listed below are
# defined in the linker script.

# These are linker script symbols that are prefixed with '__"
priv_symbols = MEMORY_BASE MEMORY_SIZE STACK
priv_symbols += ENABLE_XPA
priv_symbols += FLUSH_TO_ZERO
priv_symbols += FLASH_START APP_START FLASH_APP_START
priv_symbols += ISR_VEC_SPACE ISR_VECTOR_COUNT

# A bit of makefile magic:
# foreach symbol in overridable ld-symbols :
#   If symbol has a value, produce a linker argument for that symbol.
MIPS_HAL_LDFLAGS = $(foreach a,$(priv_symbols),$(if $($a),-Wl$(comma)--defsym$(comma)__$(call lowercase,$(a))=$($a)))

ifeq ($(ROMABLE),1)
  MIPS_HAL_LDFLAGS += -T bootcode.ld
endif

# define build specific options
# Remove -std=gnu99 once the MIPS toolchain headers are updated to include upstream
# newlib commit 81c17949f0419d1c4fee421c60987ea1149522ae
# https://cygwin.com/git/gitweb.cgi?p=newlib-cygwin.git;a=commitdiff;h=81c17949f0419d1c4fee421c60987ea1149522ae
# Otherwise we get an error about a missing declaration of strnlen in some parts.
ifeq (, $(filter -std=%, $(CFLAGS)))
  CFLAGS += -std=gnu99
endif
CFLAGS_CPU   = -EL -mabi=$(ABI)
CFLAGS_LINK  = -ffunction-sections -fno-builtin -fshort-enums -fdata-sections
CFLAGS_DBG   ?= -g3
CFLAGS_OPT   ?= -Os

CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_OPT) $(CFLAGS_DBG)
CFLAGS += -DCPU_MODEL_$(call uppercase_and_underscore,$(CPU_MODEL))

ifeq ($(USE_HARD_FLOAT),1)
  CFLAGS += -mhard-float -DMIPS_HARD_FLOAT
else
  #hard-float is the default so we must set soft-float
  CFLAGS += -msoft-float
  LINKFLAGS += -msoft-float
endif

ifeq ($(USE_DSP),1)
  CFLAGS += -mdsp -DMIPS_DSP
endif

ifeq ($(TOOLCHAIN),llvm)
  # The MIPS toolchain headers in assembly mode are not compatible with Clang
  CCAS = $(PREFIX)gcc
  CCASUWFLAGS += -target $(TARGET_ARCH)
endif

ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_OPT) $(CFLAGS_DBG)

LINKFLAGS += $(MIPS_HAL_LDFLAGS)
LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts
LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT)
LINKFLAGS += -Wl,--gc-sections

OPTIONAL_CFLAGS_BLACKLIST += -Wformat-overflow
OPTIONAL_CFLAGS_BLACKLIST += -Wformat-truncation
OPTIONAL_CFLAGS_BLACKLIST += -gz
