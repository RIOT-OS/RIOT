# Set USB VID/PID via CFLAGS if not being set via Kconfig
ifndef CONFIG_USB_VID
  ifdef USB_VID
    CFLAGS += -DCONFIG_USB_VID=0x$(USB_VID)
  endif
else
  USB_VID = $(patsubst 0x%,%,$(CONFIG_USB_VID))
endif

ifndef CONFIG_USB_PID
  ifdef USB_PID
    CFLAGS += -DCONFIG_USB_PID=0x$(USB_PID)
  endif
else
  USB_PID = $(patsubst 0x%,%,$(CONFIG_USB_PID))
endif

# Exported for the benefit of Kconfig
USB_VID_TESTING = 1209
USB_PID_TESTING = 7D01
usb_id_check:
	@if grep -q -i "^$(USB_VID) $(USB_PID)$$" $(RIOTBASE)/dist/usb_id_testing; then \
		$(COLOR_ECHO) "$(COLOR_RED)Private testing pid.codes USB VID/PID used!, do not use it outside of test environments!$(COLOR_RESET)" 1>&2 ; \
		$(COLOR_ECHO) "$(COLOR_RED)MUST NOT be used on any device redistributed, sold or manufactured, VID/PID is not unique!$(COLOR_RESET)" 1>&2 ; \
	fi
	@if [ "$(USB_VID) $(subst D,d,$(USB_PID))" = "1209 7d00" ]; then \
		$(COLOR_ECHO) "$(COLOR_RED)RIOT standard peripherals code (1209/7D00) cannot be set explicitly.$(COLOR_RESET)" 1>&2 ; \
		$(COLOR_ECHO) "$(COLOR_RED)Unset USB_VID / USB_PID for the code to be picked automatically, or set$(COLOR_RESET)" 1>&2 ; \
		$(COLOR_ECHO) "$(COLOR_RED)them to \$${USB_VID_TESTING} / \$${USB_PID_TESTING} during development.$(COLOR_RESET)" 1>&2 ; \
		exit 1; \
	fi
.PHONY: usb_id_check
all: | usb_id_check
