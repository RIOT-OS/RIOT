OPENOCD_ADAPTER_INIT ?= -c 'set stlink_version $(STLINK_VERSION);source $(RIOTBASE)/dist/tools/openocd/adapters/stlink-dap.cfg'

OPENOCD_TRANSPORT ?= dapdirect_swd

include $(RIOTMAKE)/tools/openocd-adapters/stlink.inc.mk
