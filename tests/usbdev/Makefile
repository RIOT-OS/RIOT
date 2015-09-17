APPLICATION = usbtest
include ../Makefile.tests_common

# If no BOARD is found in the environment, use this default:
BOARD = pba-d-01-kw2x
#BOARD = frdm-k64f

CFLAGS += -I$(CURDIR)

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
export QUIET = 0

# Modules to include:
USEMODULE += uart
USEMODULE += shell
USEMODULE += usbdev
USEMODULE += usbdev-acm
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
