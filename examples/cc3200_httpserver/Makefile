# name of your application
APPLICATION = cc3200-httpserver

# If no BOARD is found in the environment, use this default:
BOARD ?= cc3200-launchxl

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
QUIET ?= 0

USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include

# TI cc3200-sdk is not compliant with following RIOT constraints
CFLAGS += -Wno-strict-prototypes -Wno-old-style-definition
