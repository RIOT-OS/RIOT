BOARD ?= samr21-xpro
include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += usbus_cdc_ecm
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# USB device vendor and product ID
# pid.codes test VID/PID, not globally unique
export DEFAULT_VID = 1209
export DEFAULT_PID = 7D00
USB_VID ?= $(DEFAULT_VID)
USB_PID ?= $(DEFAULT_PID)

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

# There is a Kconfig in the app folder, we need to indicate not to run it by default
SHOULD_RUN_KCONFIG ?=

.PHONY: usb_id_check
usb_id_check:
	@if [ $(USB_VID) = $(DEFAULT_VID) -o $(USB_PID) = $(DEFAULT_PID) ] ; then \
		$(COLOR_ECHO) "$(COLOR_RED)Private testing pid.codes USB VID/PID used!, do not use it outside of test environments!$(COLOR_RESET)" 1>&2 ; \
		$(COLOR_ECHO) "$(COLOR_RED)MUST NOT be used on any device redistributed, sold or manufactured, VID/PID is not unique!$(COLOR_RESET)" 1>&2 ; \
	fi

all: | usb_id_check
