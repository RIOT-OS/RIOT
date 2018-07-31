# Olimex ARM-USB-OCD-H with ARM-JTAG-SWD adapter
OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/ftdi/olimex-arm-usb-ocd-h.cfg]' -c 'source [find interface/ftdi/olimex-arm-jtag-swd.cfg]'
# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'ftdi_serial $(DEBUG_ADAPTER_ID)'
endif
export OPENOCD_ADAPTER_INIT
