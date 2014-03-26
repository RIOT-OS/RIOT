
# download and unzip locations and names
STM_LIB_URL = http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stm32f30x_dsp_stdperiph_lib.zip
STM_LIB_ZIPFOLDER = STM32F3*
STM_LIB_TEMP = /tmp/STMF3_tmp/

# special handling of sysinit code
SYSINIT_FILE = system_stm32f30x
SYSINIT_PATH = $(STM_LIB_BASE)lib/Libraries/CMSIS/Device/ST/STM32F30x/Source/Templates/

# collect objects that will be build
LIB_BINDIR = $(STM_LIB_BASE)bin/
SRC = $(wildcard $(STM_LIB_SRCDIR)*.c)
SRCCLEAN = $(notdir $(SRC))
OBJ = $(SRCCLEAN:%.c=$(LIB_BINDIR)%.o)
OBJ += $(LIB_BINDIR)$(SYSINIT_FILE).o

# define the module constant (but this should not be needed)
MODULE = $(STM_LIB_NAME)

.PHONY: all clean

all: $(STM_LIB_BASE)lib $(BINDIR)$(STM_LIB_NAME).a

$(BINDIR)$(STM_LIB_NAME).a: $(OBJ)
	@$(AR) -rc $(BINDIR)$(STM_LIB_NAME).a $(OBJ)
	
$(LIB_BINDIR)$(SYSINIT_FILE).o: $(SYSINIT_PATH)$(SYSINIT_FILE).c
	$(info    [CC]   $(SYSINIT_PATH)$(SYSINIT_FILE).c)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $(SYSINIT_PATH)$(SYSINIT_FILE).c -o $(LIB_BINDIR)$(SYSINIT_FILE).o

$(LIB_BINDIR)%.o: $(STM_LIB_SRCDIR)%.c
	@mkdir -p $(LIB_BINDIR)
	$(info    [CC]   $(STM_LIB_SRCDIR)$*.c)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $(STM_LIB_SRCDIR)$*.c -o $(LIB_BINDIR)$*.o

# see if the library is already present, otherwise download it
$(STM_LIB_BASE)lib:
	$(shell if [ ! -d $(STM_LIB_BASE)lib ]; then \
	  echo "  [STLIB] the STM standard periphial library for the STM32F3 is missing, will download it now from ST" ; \
	  if [ -d $(STM_LIB_TEMP) ]; then rm -r -f $(STM_LIB_TEMP); fi ; \
	  mkdir $(STM_LIB_TEMP); \
	  wget -O $(STM_LIB_TEMP)stmlib.zip $(STM_LIB_URL) ; \
	  unzip -q -d $(STM_LIB_TEMP) $(STM_LIB_TEMP)stmlib.zip ; \
	  mv $(STM_LIB_TEMP)$(STM_LIB_ZIPFOLDER) $(STM_LIB_BASE)/lib ; \
	  rm -r -f $(STM_LIB_TEMP) ; \
	  echo "  [STLIB] successfully aquired STM lib" ; \
	 fi )
	touch -c $@

clean:
	@rm -r -f $(STM_LIB_BASE)/bin
