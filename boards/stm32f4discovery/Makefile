SRC = $(wildcard *.c)
BINDIR = bin/
OBJ = $(SRC:%.c=$(BINDIR)%.o)## defines
export ARCH = stm32f4discovery_base.a

DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES += -I$(RIOTBOARD)/stm32f4discovery/include/
INCLUDES += -I$(RIOTBASE)/cpu/stm32_common/STM32F4xx_DSP_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Include/
INCLUDES += -I$(RIOTBASE)/cpu/stm32_common/STM32F4xx_DSP_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include/
INCLUDES += -I$(RIOTBASE)/cpu/stm32_common/STM32F4xx_DSP_StdPeriph_Lib_V1.1.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/


all: $(RIOTBASE)/cpu/stm32_common/STM32F4xx_DSP_StdPeriph_Lib_V1.1.0 $(BINDIR)$(ARCH)
	#$(MAKE) -C drivers
	
$(RIOTBASE)/cpu/stm32_common/STM32F4xx_DSP_StdPeriph_Lib_V1.1.0:
	$(MAKE) -C $(RIOTBASE)/cpu/stm32_common

$(BINDIR)$(ARCH): $(OBJ)
	@$(AR) rcs $(BINDIR)$(ARCH) $(OBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	@mkdir -p $(BINDIR)
	@$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -c $*.c -o $(BINDIR)$*.o
	@$(CC) $(CFLAGS) $(INCLUDES) $(BOARDINCLUDE) $(PROJECTINCLUDE) $(CPUINCLUDE) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/riot_out && mv /tmp/riot_out $(BINDIR)$*.d

# remove compilation products
clean:
	rm -f $(BINDIR)$(ARCH) $(OBJ) $(DEP)
	@if [ -d $(BINDIR) ] ; \
	then rmdir $(BINDIR) ; \
	fi 


