# Target architecture for the build. Use msp430-elf if you are unsure.
TARGET_ARCH_MSP430 ?= msp430-elf
TARGET_ARCH ?= $(TARGET_ARCH_MSP430)

MSP430_SUPPORT_FILES ?= $(RIOTCPU)/msp430_common/vendor/msp430-gcc-support-files

# define build specific options
CFLAGS_CPU   = -mmcu=$(CPU_MODEL) -isystem $(MSP430_SUPPORT_FILES)/include
CFLAGS_LINK  = -ffunction-sections -fdata-sections
CFLAGS_DBG  ?= -g -gdwarf-2
CFLAGS_OPT  ?= -Os

CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)
ASFLAGS += $(CFLAGS_CPU) --defsym $(CPU_MODEL)=1 $(CFLAGS_DBG)

LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT)
LINKFLAGS += -Wl,--gc-sections -Wl,-L$(MSP430_SUPPORT_FILES)/include
LINKFLAGS += -T $(MSP430_SUPPORT_FILES)/include/$(CPU_MODEL).ld
LINKFLAGS += -T $(RIOTCPU)/msp430_common/ldscripts/msp430_common.ld
LINKFLAGS += $(RIOTCPU)/msp430_common/ldscripts/xfa.ld

OPTIONAL_CFLAGS_BLACKLIST += -fdiagnostics-color
OPTIONAL_CFLAGS_BLACKLIST += -Wformat-overflow
OPTIONAL_CFLAGS_BLACKLIST += -Wformat-truncation
OPTIONAL_CFLAGS_BLACKLIST += -gz
