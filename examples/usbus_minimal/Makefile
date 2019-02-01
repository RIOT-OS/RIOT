# name of your application
APPLICATION = usbus_minimal

# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

USEMODULE += usbus

# USB device vendor and product ID
USB_VID ?= 1209
USB_PID ?= 0001

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

CFLAGS += -DUSB_CONFIG_VID=0x$(USB_VID) -DUSB_CONFIG_PID=0x$(USB_PID)

include $(RIOTBASE)/Makefile.include

ifeq ($(USB_VID):$(USB_PID), 1209:0001)
  $(shell $(COLOR_ECHO) "$(COLOR_RED)Private testing pid.codes USB VID/PID used!, do not use it outside of test environments!$(COLOR_RESET)" 1>&2)
  $(shell $(COLOR_ECHO) "$(COLOR_RED)MUST NOT be used on any device redistributed, sold or manufactured, VID/PID is not unique!$(COLOR_RESET)" 1>&2)
endif
