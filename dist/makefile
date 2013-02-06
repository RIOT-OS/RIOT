####
#### Sample Makefile for building apps with the RIOT OS
####
#### The Sample Filesystem Layout is:
#### /this makefile
#### ../RIOT 
#### ../RIOT/board   for board definitions (if you have one or more)
#### 

#### Mandatory vars!

# this has to be the absolute path of the feuerware-base dir
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
LINKFLAGS = -gdwarf-2 -mcpu=arm7tdmi-s -static -lgcc -nostartfiles -T$(RIOTBASE)/cpu/$(CPU)/linkerscript.x

## Modules to include. No autodependencies!

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

# mandatory include! 
include $(RIOTBASE)/makefile.modules 

#### Project Config 

# name of your project
PROJECT = hello-world

# for easy switching of boards
export BOARD = msba2

# if you want to publish the board into the sources as an uppercase #define
BB = $(shell echo $(BOARD)|tr 'a-z' 'A-Z')
CFLAGS += -DBOARD_$(BB)
export CFLAGS

# your binaries to link
BASELIBS += $(RIOTBOARD)/$(BOARD)/bin/$(BOARD)_base.a
BASELIBS += bin/project.a

INCLUDES = -I$(RIOTBOARD)/$(BOARD)/include -I$(RIOTBASE)/core/include -I$(RIOTBASE)/cpu/$(CPU)/include -I$(RIOTBASE)/cpu/arm_common/include -I$(RIOTBASE)/sys/lib

export INCLUDES

## make script for your project. Build feuerware-base here!
all: bin/$(PROJECT).a
	$(MAKE) -C $(RIOTBOARD)
	$(MAKE) -C $(RIOTBASE)
	$(LINK) $(LINKFLAGS) $(UNDEF) -o bin/$(PROJECT).elf -Wl,--start-group $(BASELIBS) -lm -Wl,--end-group  -Wl,-Map=bin/$(PROJECT).map
	$(SIZE) bin/$(PROJECT).elf
	$(OBJCOPY) -O ihex bin/$(PROJECT).elf bin/$(PROJECT).hex

## your make rules
## Only basic example - modify it for larger projects!!

bin/$(PROJECT).a: bin/$(PROJECT).o
	$(AR) -rc bin/project.a bin/$(PROJECT).o 
	
bin/$(PROJECT).o: main.c 
	$(CC) $(CFLAGS) $(BOARDINCLUDE) $(INCLUDES) -c main.c -o bin/$(PROJECT).o

clean:
	$(MAKE) -C $(RIOTBOARD) clean
	$(MAKE) -C $(RIOTBASE) clean
	rm -f bin/*
	
