# name of your application
APPLICATION = default

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../RIOT/thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../RIOT/thirdparty_boards

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:

USEMODULE += uart0
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += defaulttransceiver
USEMODULE += config

FEATURES_OPTIONAL += transceiver
FEATURES_OPTIONAL += config
FEATURES_OPTIONAL += periph_rtc

ifneq (,$(filter msb-430,$(BOARD)))
	USEMODULE += sht11
endif
ifneq (,$(filter msba2,$(BOARD)))
	USEMODULE += sht11
	USEMODULE += ltc4150
	USEMODULE += mci
	USEMODULE += random
endif
ifneq (,$(filter native,$(BOARD)))
	USEMODULE += ltc4150
	USEMODULE += random
endif
ifneq (,$(filter iot-lab_M3,$(BOARD)))
	USEMODULE += isl29020
	USEMODULE += lps331ap
	USEMODULE += l3g4200d
	USEMODULE += lsm303dlhc
endif
ifneq (,$(filter fox,$(BOARD)))
        USEMODULE += lps331ap
        USEMODULE += l3g4200d
	USEMODULE += lsm303dlhc
endif

include $(RIOTBASE)/Makefile.include
