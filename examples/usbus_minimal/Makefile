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
USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

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
