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
		$(call sh_echoerr,("Private testing pid.codes USB VID/PID used!, do not"\
	                     "use it outside of test environments!")); \
		$(call sh_echoerr,("MUST NOT be used on any device redistributed,"\
	                     "sold or manufactured, VID/PID is not unique!")); \
	fi
	@if [ "$(USB_VID) $(subst D,d,$(USB_PID))" = "1209 7d00" ]; then \
		$(call sh_echoerr,("RIOT standard peripherals code \(1209/7D00\) cannot be set explicitly.")); \
		$(call sh_echoerr,("Unset USB_VID / USB_PID for the code to be picked automatically, or set")); \
		$(call sh_echoerr,("them to \$${USB_VID_TESTING} / \$${USB_PID_TESTING} during development.")); \
		exit 1; \
	fi
.PHONY: usb_id_check
all: | usb_id_check
