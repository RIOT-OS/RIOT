# name of your application
APPLICATION = riot_and_cpp

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# stm32f0discovery objects are too big with ARM Embedded Toolchain v4.9.3 20141119
# (used currently by travis)
BOARD_INSUFFICIENT_MEMORY=stm32f0discovery

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Uncomment these lines if you want to use platform support from external
# repositories:
#RIOTCPU ?= $(CURDIR)/../../../thirdparty_cpu
#RIOTBOARD ?= $(CURDIR)/../../../thirdparty_boards

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

# Features required
FEATURES_REQUIRED += cpp

# If you want to add some extra flags when compile c++ files, add these flags
# to CXXEXFLAGS variable
CXXEXFLAGS +=

include $(RIOTBASE)/Makefile.include
