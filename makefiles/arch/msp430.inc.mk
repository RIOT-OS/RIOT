# Target architecture for the build. Use msp430 if you are unsure.
TARGET_ARCH ?= msp430

# define build specific options
CFLAGS_CPU   = -mmcu=$(CPU_MODEL)
# default std set to gnu99 of not overwritten by user
ifeq (, $(filter -std=%, $(CFLAGS)))
  CFLAGS += -std=gnu99
endif
CFLAGS_LINK  = -ffunction-sections -fdata-sections
CFLAGS_DBG  ?= -gdwarf-2
CFLAGS_OPT  ?= -Os
CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)
ASFLAGS += $(CFLAGS_CPU) --defsym $(CPU_MODEL)=1 $(CFLAGS_DBG)
# export linker flags
LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) -Wl,--gc-sections -static -lgcc

OPTIONAL_CFLAGS_BLACKLIST += -fdiagnostics-color
OPTIONAL_CFLAGS_BLACKLIST += -Wformat-overflow
OPTIONAL_CFLAGS_BLACKLIST += -Wformat-truncation
OPTIONAL_CFLAGS_BLACKLIST += -gz
