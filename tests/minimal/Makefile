# name of your application
APPLICATION = minimal

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

#
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

#
DISABLE_MODULE += auto_init

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
