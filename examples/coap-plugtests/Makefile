# name of your application
APPLICATION = coap-plugtests

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Comment this out to disable scheduler statistics for ps:
CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# assert.h missing
BOARD_BL_ASSERT := chronos msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1
# 'sysconfig' undeclared
BOARD_BL_SYSCONFIG := samr21-xpro fox iot-lab_M3

BOARD_BLACKLIST := $(BOARD_BL_ASSERT) $(BOARD_BL_SYSCONFIG)

# undefined reference to `_gettimeofday'`
BOARD_LD_ERROR := redbee-econotag msba2 msbiot avsextrem

BOARD_INSUFFICIENT_RAM := $(BOARD_LD_ERROR)

# Modules to include:

USEMODULE += uart0
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += defaulttransceiver
USEMODULE += udp
USEMODULE += config

USEPKG += libcoap

include $(RIOTBASE)/Makefile.include
