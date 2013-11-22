
SRCDIR = STM32F4xx_DSP_StdPeriph_Lib_V1.1.0/Libraries/STM32F4xx_StdPeriph_Driver/src/
BINDIR = $(RIOTBASE)/bin/
SRC = $(wildcard $(SRCDIR)*.c)
SRCCLEAN = $(notdir $(SRC))
OBJ = $(SRCCLEAN:%.c=$(BINDIR)%.o)

INCLUDES += -I$(RIOTBASE)/cpu/stm32_common/STM32F4xx_DSP_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Include/
INCLUDES += -I$(RIOTBASE)/cpu/stm32_common/STM32F4xx_DSP_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include/
INCLUDES += -I$(RIOTBASE)/cpu/stm32_common/STM32F4xx_DSP_StdPeriph_Lib_V1.1.0/Libraries/STM32F4xx_StdPeriph_Driver/inc/
INCLUDES += -include $(RIOTBOARD)/$(BOARD)/include/board-conf.h
.PHONY: Stm32f4StdPeriphDriver
.PHONY: stmobj
MODULE = Stm32f4StdPeriphDriver 

# call makefile again to update $(SRC)	
Stm32f4StdPeriphDriver: STM32F4xx_DSP_StdPeriph_Lib_V1.1.0 
	@$(MAKE) STMOBJ 

STMOBJ: $(OBJ)
	@$(AR) -rc $(BINDIR)Stm32f4StdPeriphDriver.a $(OBJ)
		
$(BINDIR)%.o: $(SRCDIR)%.c
	@test -d $(BINDIR) || mkdir -p $(BINDIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)$*.c -o $(BINDIR)$*.o

STM32F4xx_DSP_StdPeriph_Lib_V1.1.0: stm32f4_dsp_stdperiph_lib.zip
	unzip -oq stm32f4_dsp_stdperiph_lib.zip
	touch -c $@

stm32f4_dsp_stdperiph_lib.zip:
	wget http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stm32f4_dsp_stdperiph_lib.zip
	touch -c $@

clean:
	@rm -f $(BINDIR)Stm32f4StdPeriphDriver.a $(OBJ) 








