# Set the name of your application:
export APPLICATION = osci

# If no BOARD is found in the environment, use this default:
export BOARD ?= msba2

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../../RIOT

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

USEMODULE += auto_init
USEMODULE += vtimer 
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include

