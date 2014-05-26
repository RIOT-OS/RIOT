MODULE =cpu

OBJECTS = system_LPC17xx.o startup_LPC17xx.o main.o

CC_SYMBOLS = -DTARGET_LPC1769 -DTOOLCHAIN_GCC_ARM -DNDEBUG -D__CORTEX_M3

LD_FLAGS = -mcpu=cortex-m3 -mthumb -Wl,--gc-sections,-Map=$(APPLICATION).map,--cref --specs=nano.specs
LD_SYS_LIBS = -lc -lgcc -lnosys

all: $(BINDIR)$(MODULE).a

include $(RIOTBASE)/Makefile.base

# This is needed for NXP Cortex M devices
nxpsum:
	$(CCLOCAL) nxpsum.c -std=c99 -o nxpsum

#$(APPLICATION).elf: $(OBJECTS) $(SYS_OBJECTS)
#	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $(addprefix $(BUILD_DIR), $^) $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)

$(APPLICATION).bin: $(APPLICATION).elf nxpsum
	$(OBJCOPY) -O binary $< $@
	# Compute nxp checksum on .bin file here
	./nxpsum $@
