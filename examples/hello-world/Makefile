# name of your application
APPLICATION = hello-world

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Disable support for multi-tasking, which is not needed for this trivial
# application
DISABLE_MODULE += multi_tasking core_msg

include $(RIOTBASE)/Makefile.include
