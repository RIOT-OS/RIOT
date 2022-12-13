# TI XDS110 debug adapter (available both as embedded debugger e.g. in the
# cc13xx-launchpad and cc26xx-launchpad boards or standalone version)

OPENOCD_TRANSPORT ?= jtag

OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/xds110.cfg]'
