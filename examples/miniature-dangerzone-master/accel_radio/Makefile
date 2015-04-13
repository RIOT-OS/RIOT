####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../../RIOT 
#### 

# name of your project
export APPLICATION = accel_radio

export PORT =/dev/ttyUSB0

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD =avsextrem
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE =$(CURDIR)/../../RIOT

## Modules to include. 

USEMODULE += auto_init
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += uart0
USEMODULE += posix
USEMODULE += ps 
USEMODULE += vtimer 
USEMODULE += cc110x_ng
USEMODULE += transceiver
mSEMODULE += gpioint

include $(RIOTBASE)/Makefile.include

$(info *******************************************)
$(info **** Building Project $(PROJECT) ****)
$(info *******************************************)
$(info **** BOARD $(BOARD))
$(info **** BASE:  $(RIOTBASE))
$(info **** CPU:   $(RIOTCPU))
$(info **** BOARD: $(RIOTBOARD))
$(info *************************************)
$(info **** MODULES:)
$(foreach module,$(USEMODULE),$(info ++++           $(module)))
$(info **** INCLUDES:)
$(foreach include,$(INCLUDES),$(info ++++           $(include)))
$(info **** BASELIBS:)
$(foreach lib,$(BASELIBS),$(info ++++           $(lib)))
$(info **** SRC:)
$(foreach srcfile,$(SRC),$(info ++++           $(srcfile)))
$(info **** OBJ:)
$(foreach objfile,$(OBJ),$(info ++++           $(objfile)))
$(info **** CC: $(CC))
$(foreach flag,$(CFLAGS),$(info ++++            $(flag)))
