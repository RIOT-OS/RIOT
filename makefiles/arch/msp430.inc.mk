# Target architecture for the build. Use msp430-elf if you are unsure.
TARGET_ARCH_MSP430 ?= msp430-elf
TARGET_ARCH ?= $(TARGET_ARCH_MSP430)

MSP430_SUPPORT_FILES ?= $(RIOTCPU)/msp430/vendor/msp430-gcc-support-files

# define build specific options
CFLAGS_CPU   = -mmcu=$(CPU_MODEL) -isystem $(MSP430_SUPPORT_FILES)/include
CFLAGS_LINK  = -ffunction-sections -fdata-sections
CFLAGS_DBG  ?= -g -gdwarf-2
CFLAGS_OPT  ?= -Os

CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)
ASFLAGS += $(CFLAGS_CPU) --defsym $(CPU_MODEL)=1 $(CFLAGS_DBG)

BINUTILS_VERSION := $(shell $(PREFIX)ld --version | grep -Eo '[0-9]\.[0-9]+$$')
NEEDS_NEW_LINKER_SCRIPT := $(call version_is_greater_or_equal,$(BINUTILS_VERSION),2.40)

LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT)
LINKFLAGS += -Wl,--gc-sections
LINKFLAGS += -Wl,-L$(MSP430_SUPPORT_FILES)/include
LINKFLAGS += -Wl,-L$(RIOTCPU)/msp430/ldscripts

ifeq (1,$(NEEDS_NEW_LINKER_SCRIPT))
  LINKFLAGS += -T riot-$(CPU_MODEL).ld
else
  LINKFLAGS += -T $(MSP430_SUPPORT_FILES)/include/$(CPU_MODEL).ld
  LINKFLAGS += -T $(RIOTCPU)/msp430/ldscripts/msp430_common.ld
  LINKFLAGS += -T $(RIOTCPU)/msp430/ldscripts/$(CPU_FAM).ld
  LINKFLAGS += $(RIOTCPU)/msp430/ldscripts/xfa.ld
endif

OPTIONAL_CFLAGS_BLACKLIST += -fdiagnostics-color
OPTIONAL_CFLAGS_BLACKLIST += -Wformat-overflow
OPTIONAL_CFLAGS_BLACKLIST += -Wformat-truncation
OPTIONAL_CFLAGS_BLACKLIST += -gz
