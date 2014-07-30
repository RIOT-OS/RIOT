# name of your application
export APPLICATION = clocksync

# If no BOARD is found in the environment, use this default:
export BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#export RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#export RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#CFLAGS += -DDEVELHELP

export LINKFLAGS += -lm

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

# Modules to include:

USEMODULE += posix
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += defaulttransceiver
USEMODULE += auto_init
USEMODULE += sixlowpan

ifneq (,$(filter msb-430,$(BOARD)))
	USEMODULE += sht11
endif
ifneq (,$(filter msba2,$(BOARD)))
	USEMODULE += sht11
	USEMODULE += ltc4150
	USEMODULE += rtc
	USEMODULE += mci
	USEMODULE += config
	USEMODULE += random
	USEMODULE += ftsp
endif
ifneq (,$(filter avsextrem,$(BOARD)))
	USEMODULE += rtc
	USEMODULE += config
	USEMODULE += random
	USEMODULE += ftsp
endif
ifneq (,$(filter native,$(BOARD)))
	USEMODULE += ltc4150
	USEMODULE += rtc
	USEMODULE += config
	USEMODULE += nativenet
	USEMODULE += random
	USEMODULE += ftsp
endif

include $(RIOTBASE)/Makefile.include
