# name of your application
APPLICATION = sixlowapp

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += uart0
USEMODULE += ps
USEMODULE += defaulttransceiver
USEMODULE += udp
ifneq (,$(filter iot-lab_M3,$(BOARD)))
	USEMODULE += isl29020
	USEMODULE += lps331ap
	USEMODULE += l3g4200d
	USEMODULE += lsm303dlhc
endif


include $(RIOTBASE)/Makefile.include
