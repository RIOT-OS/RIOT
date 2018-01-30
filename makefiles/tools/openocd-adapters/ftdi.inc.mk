# ftdi-serial debug adapter
# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'ftdi_serial $(DEBUG_ADAPTER_ID)'
else ifeq (Linux,$(OS))
  # Some boards don't have serial at all (eg. itolab-m3)
  # find-usb.sh will return the USB hardware address for PORT (/dev/ttyUSB*)
  DEBUG_ADAPTER_ID := $(firstword $(shell $(RIOTBASE)/dist/tools/usb-serial/find-usb.sh '$(PORT)'))
  OPENOCD_EXTRA_INIT += -c 'ftdi_location $(DEBUG_ADAPTER_ID)'
endif
export OPENOCD_ADAPTER_INIT
export OPENOCD_EXTRA_INIT
