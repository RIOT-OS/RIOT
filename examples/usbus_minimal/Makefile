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
USEMODULE += auto_init_usbus

# USB device vendor and product ID
export DEFAULT_VID = 1209
export DEFAULT_PID = 7D00
USB_VID ?= $(DEFAULT_VID)
USB_PID ?= $(DEFAULT_PID)

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Do not run Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

# Set USB VID/PID via CFLAGS if not being set via Kconfig
ifndef CONFIG_USB_VID
  CFLAGS += -DCONFIG_USB_VID=0x$(USB_VID)
else
  USB_VID = $(patsubst 0x%,%,$(CONFIG_USB_VID))
endif

ifndef CONFIG_USB_PID
  CFLAGS += -DCONFIG_USB_PID=0x$(USB_PID)
else
  USB_PID = $(patsubst 0x%,%,$(CONFIG_USB_PID))
endif

.PHONY: usb_id_check
usb_id_check:
	@if [ $(USB_VID) = $(DEFAULT_VID) -o $(USB_PID) = $(DEFAULT_PID) ] ; then \
		$(COLOR_ECHO) "$(COLOR_RED)Private testing pid.codes USB VID/PID used!, do not use it outside of test environments!$(COLOR_RESET)" 1>&2 ; \
		$(COLOR_ECHO) "$(COLOR_RED)MUST NOT be used on any device redistributed, sold or manufactured, VID/PID is not unique!$(COLOR_RESET)" 1>&2 ; \
	fi

all: | usb_id_check
