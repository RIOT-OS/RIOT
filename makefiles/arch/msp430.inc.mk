# Target architecture for the build. Use msp430 if you are unsure.
export TARGET_ARCH ?= msp430

# define build specific options
CFLAGS_CPU   = -mmcu=$(CPU_MODEL) -std=gnu99
CFLAGS_LINK  = -ffunction-sections -fdata-sections
CFLAGS_DBG  ?= -gdwarf-2
CFLAGS_OPT  ?= -Os
# export compiler flags
export CFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT)
# export assmebly flags
export ASFLAGS += $(CFLAGS_CPU) --defsym $(CPU_MODEL)=1 $(CFLAGS_DBG)
# export linker flags
export LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) -Wl,--gc-sections -static -lgcc
