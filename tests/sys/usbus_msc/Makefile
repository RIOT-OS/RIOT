
BOARD ?= same54-xpro
include ../Makefile.sys_common

 # This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

USEMODULE += auto_init_usbus

USEMODULE += mtd
USEMODULE += mtd_write_page
USEMODULE += ps
USEMODULE += shell
USEMODULE += usbus_msc
USEMODULE += ztimer_msec

# If your board does not provide a MTD for testing, use the following line
# to emulate an MTD with 64 sectors with 4 pages of 128 bytes each in RAM. You
# can override these parameters by SECTOR_COUNT, PAGES_PER_SECTOR and PAGE_SIZE.
# USEMODULE += mtd_emulated

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

include $(RIOTBASE)/Makefile.include
