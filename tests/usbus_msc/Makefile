
BOARD ?= same54-xpro
include ../Makefile.tests_common

 # This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

 # Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

USEMODULE += auto_init_usbus
USEMODULE += usbus_msc
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

include $(RIOTBASE)/Makefile.include
