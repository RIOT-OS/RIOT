# iotlab-usb debug adapter
OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/ftdi/iotlab-usb.cfg]'

# default to JTAG
OPENOCD_TRANSPORT ?= jtag
