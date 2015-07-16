APPLICATION = openwsn-app

# If no BOARD is found in the environment, use this default:
BOARD ?= iot-lab_M3

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

USEMODULE += ps
USEMODULE += vtimer
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += uart0
USEMODULE += at86rf231

USEPKG += openwsn

include $(RIOTBASE)/Makefile.include

INCLUDES += -I$(RIOTBASE)/pkg/openwsn/openwsn/projects/common/03oos_openwsn \
	-I$(RIOTBASE)/pkg/openwsn/openwsn/bsp/boards/riot-adaption
