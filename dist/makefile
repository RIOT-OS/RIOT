####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../RIOT 
#### ../RIOT/board   for board definitions (if you have one or more)
#### 

#### Mandatory vars!

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE =$(CURDIR)/../RIOT
export RIOTBOARD =$(CURDIR)/../RIOT/board

# the cpu to build for
export CPU = lpc2387

# toolchain config
export PREFIX = @arm-elf-
export CC = @$(PREFIX)gcc
export AR = @$(PREFIX)ar
export CFLAGS = -std=gnu99 -O2 -Wall -Wstrict-prototypes -mcpu=arm7tdmi-s -gdwarf-2
export ASFLAGS = -gdwarf-2 -mcpu=arm7tdmi-s
export AS = $(PREFIX)as
export LINK = $(PREFIX)gcc
export SIZE = $(PREFIX)size
export OBJCOPY = $(PREFIX)objcopy
FLASH = lpc2k_pgm
TERM = pyterm.py
LINKFLAGS = -gdwarf-2 -mcpu=arm7tdmi-s -static -lgcc -nostartfiles -T$(RIOTBASE)/cpu/$(CPU)/linkerscript.x

PROJBINDIR = bin

ifeq ($(strip $(PORT)),)
	PORT = /dev/ttyUSB0
endif

## Modules to include. 

#USEMODULE += nanopan5375_proprietary
#USEMODULE += sys
#USEMODULE += cmdd
#USEMODULE += logd
#USEMODULE += mprint
#USEMODULE += mqueue
#USEMODULE += sync_read
#USEMODULE += sysmon
#USEMODULE += syslog
#USEMODULE += tracelog
#USEMODULE += shell
#USEMODULE += utimer
#USEMODULE += sht11
#USEMODULE += powermon
#USEMODULE += ltc4150
#USEMODULE += sys_config
#USEMODULE += cc110x
#USEMODULE += cmdengine
#USEMODULE += ezxml
#USEMODULE += fat
#USEMODULE += gps
#USEMODULE += geo
#USEMODULE += gps_conversion
#USEMODULE += gps_ublox
#USEMODULE += net_kernel
#USEMODULE += net_mm

#### Project Config 

# name of your project
PROJECT = hello-world

# for easy switching of boards
export BOARD = msba2

# mandatory include! 
include $(RIOTBASE)/makefile.modules 

# if you want to publish the board into the sources as an uppercase #define
BB = $(shell echo $(BOARD)|tr 'a-z' 'A-Z')
CFLAGS += -DBOARD_$(BB)
export CFLAGS

# your binaries to link
BASELIBS += $(RIOTBOARD)/$(BOARD)/bin/$(BOARD)_base.a
BASELIBS += $(PROJBINDIR)/project.a

INCLUDES = -I$(RIOTBOARD)/$(BOARD)/include -I$(RIOTBASE)/core/include -I$(RIOTBASE)/cpu/$(CPU)/include -I$(RIOTBASE)/cpu/arm_common/include -I$(RIOTBASE)/sys/lib -I$(RIOTBASE)/sys/include/ -I$(RIOTBASE)/drivers/include/

export INCLUDES

## make script for your project. Build RIOT-base here!
all: $(PROJBINDIR)/$(PROJECT).a
	@echo "Building project $(PROJECT) for $(BOARD) w/ MCU $(CPU)."
	$(MAKE) -C $(RIOTBOARD)
	$(MAKE) -C $(RIOTBASE)
	$(LINK) $(LINKFLAGS) $(UNDEF) -o $(PROJBINDIR)/$(PROJECT).elf -Wl,--start-group $(BASELIBS) -lm -Wl,--end-group  -Wl,-Map=$(PROJBINDIR)/$(PROJECT).map
	$(SIZE) $(PROJBINDIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $(PROJBINDIR)/$(PROJECT).elf $(PROJBINDIR)/$(PROJECT).hex

## your make rules
## Only basic example - modify it for larger projects!!

$(PROJBINDIR)/$(PROJECT).a: $(PROJBINDIR)/$(PROJECT).o
	$(AR) -rc $(PROJBINDIR)/project.a $(PROJBINDIR)/$(PROJECT).o 
	
$(PROJBINDIR)/$(PROJECT).o: main.c 
	$(CC) $(CFLAGS) $(BOARDINCLUDE) $(INCLUDES) -c main.c -o $(PROJBINDIR)/$(PROJECT).o

clean:
	$(MAKE) -C $(RIOTBOARD) clean
	$(MAKE) -C $(RIOTBASE) clean
	rm -f $(PROJBINDIR)/*

flash: all
	$(FLASH) $(PORT) $(PROJBINDIR)/$(PROJECT).hex

term:
	$(TERM) $(PORT)
