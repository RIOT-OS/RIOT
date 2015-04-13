# Set the name of your application:
export PROJECT = latency

# If no BOARD is found in the environment, use this default:
export BOARD ?= msb-430h

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../../RIOT

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

USEMODULE += vtimer 
USEMODULE += ps
ifneq (,$(filter msba2,$(BOARD)))
	USEMODULE += gpioint
endif

include $(RIOTBASE)/Makefile.include

