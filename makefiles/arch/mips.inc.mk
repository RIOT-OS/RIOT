# Target triple for the build.
export TARGET_ARCH ?= mips-mti-elf

export ABI=32

# Portable 'lowercase' func.
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

# Default values for the linker script symbols listed below are
# defined in the linker script.

# These are linker script symbols that are prefixed with '__"
priv_symbols = MEMORY_BASE MEMORY_SIZE STACK
priv_symbols += ENABLE_XPA
priv_symbols += FLUSH_TO_ZERO
priv_symbols += FLASH_START APP_START FLASH_APP_START
priv_symbols += ISR_VEC_SPACE ISR_VECTOR_COUNT

comma := ,

# A bit of makefile magic:
# foreach symbol in overridable ld-symbols :
#   If symbol has a value, produce a linker argument for that symbol.
MIPS_HAL_LDFLAGS = $(foreach a,$(priv_symbols),$(if $($a),-Wl$(comma)--defsym$(comma)__$(call lc,$(a))=$($a)))

ifeq ($(ROMABLE),1)
MIPS_HAL_LDFLAGS += -T bootcode.ld
endif

# define build specific options
# Remove -std=gnu99 once the MIPS toolchain headers are updated to include upstream
# newlib commit 81c17949f0419d1c4fee421c60987ea1149522ae
# https://cygwin.com/git/gitweb.cgi?p=newlib-cygwin.git;a=commitdiff;h=81c17949f0419d1c4fee421c60987ea1149522ae
# Otherwise we get an error about a missing declaration of strnlen in some parts.
ifeq (, $(filter -std=%, $(CFLAGS)))
  export CFLAGS += -std=gnu99
endif
export CFLAGS_CPU   = -EL -mabi=$(ABI)
export CFLAGS_LINK  = -ffunction-sections -fno-builtin -fshort-enums -fdata-sections
export CFLAGS_DBG   = -g3
export CFLAGS_OPT   = -Os

export CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_OPT) $(CFLAGS_DBG)

ifeq ($(USE_HARD_FLOAT),1)
    export CFLAGS += -mhard-float -DMIPS_HARD_FLOAT
else
    #hard-float is the default so we must set soft-float
    export CFLAGS += -msoft-float
    export LINKFLAGS += -msoft-float
endif

ifeq ($(USE_DSP),1)
    export CFLAGS += -mdsp -DMIPS_DSP
endif

ifeq ($(TOOLCHAIN),llvm)
# The MIPS toolchain headers in assembly mode are not compatible with Clang
export CCAS = $(PREFIX)gcc
export CCASUWFLAGS += -target $(TARGET_ARCH)
endif

export ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_OPT) $(CFLAGS_DBG)

export LINKFLAGS += $(MIPS_HAL_LDFLAGS)
export LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts
export LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT)
export LINKFLAGS += -Wl,--gc-sections
