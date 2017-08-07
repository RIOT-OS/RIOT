# Ameya Joshi
# name of your application
APPLICATION = udp-server

# If no BOARD is found in the environment, use this default:
BOARD ?= cc3200-launchxl

#BURBA ?= $(CURDIR)/../..
RIOTBASE ?= ${HOME}/RIOT-master

DIRS += $(CURDIR)/../../sys
#DIRS += $(CURDIR)/../sys


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += tcp_cc3200

USEMODULE += sbapp_cc3200

USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
