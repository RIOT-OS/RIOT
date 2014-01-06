# name of your project
export PROJECT = default

# for easy switching of boards
ifeq ($(strip $(BOARD)),)
	export BOARD = native
endif

# this has to be the absolute path of the RIOT-base dir
export RIOTBASE = $(CURDIR)/../..

# uncomment this to enable scheduler statistics for ps
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use valgrind, you should recompile native with either
# HAVE_VALGRIND_H or HAVE_VALGRIND_VALGRIND_H depending on the location
# of the valgrind header (i.e. <valgrind.h> or <valgrind/valgrind.h>)
# For more information about the valgrind support of RIOT read:
# RIOT/cpu/native/README
#CFLAGS += -DHAVE_VALGRIND_VALGRIND_H
#CFLAGS += -DHAVE_VALGRIND_H

USEMODULE += posix
USEMODULE += uart0
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += auto_init
ifneq (,$(findstring msb-430,$(BOARD)))
	USEMODULE += sht11
endif
ifneq (,$(findstring chronos,$(BOARD)))
	USEMODULE += cc110x_ng
	export INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include
endif
ifneq (,$(findstring wsn430-v1_3b,$(BOARD)))
	USEMODULE += cc110x_ng
	export INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include
endif
ifneq (,$(findstring wsn430-v1_4,$(BOARD)))
	USEMODULE += cc2420
	export INCLUDES += -I$(RIOTBASE)/drivers/cc2420/include
endif
ifneq (,$(findstring msb-430h,$(BOARD)))
	USEMODULE += cc110x_ng
	export INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include
endif
ifneq (,$(findstring msba2,$(BOARD)))
	USEMODULE += sht11
	USEMODULE += ltc4150
	USEMODULE += rtc
	USEMODULE += mci
	USEMODULE += cc110x_ng
	USEMODULE += config
	export INCLUDES += -I$(RIOTBASE)/drivers/cc110x_ng/include
endif
ifneq (,$(findstring native,$(BOARD)))
	USEMODULE += ltc4150
	USEMODULE += rtc
	USEMODULE += nativenet
	USEMODULE += transceiver
	USEMODULE += config
endif

export INCLUDES += -I${RIOTBASE}/core/include/ -I${RIOTBASE}/sys/include/ -I${RIOTBASE}/drivers/include/

include $(RIOTBASE)/Makefile.include
