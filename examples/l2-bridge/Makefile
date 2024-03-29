# name of your application
APPLICATION = l2-bridge

# If no BOARD is found in the environment, use this default:
BOARD ?= same54-xpro

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += event_callback
USEMODULE += event_thread

USEMODULE += sam0_eth
USEMODULE += usbus_cdc_ecm

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps

# USB device vendor and product ID
USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

-include Makefile.$(BOARD).conf
include $(RIOTBASE)/Makefile.include
