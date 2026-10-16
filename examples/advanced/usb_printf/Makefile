# name of your application
APPLICATION = usb_printf

# This example targets the RP2350 (Arm). Any board providing periph_usbdev works.
BOARD ?= rpi-pico-2-arm

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Use the USB CDC ACM interface as stdio (stdout/stderr)
USEMODULE += stdio_cdc_acm

# USB device vendor and product ID (testing IDs, do not redistribute)
USB_VID ?= $(USB_VID_TESTING)
USB_PID ?= $(USB_PID_TESTING)

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
