# sysfs GPIO debug adapter

SWCLK_PIN ?= 21
SWDIO_PIN ?= 20
SRST_PIN ?= 16

OPENOCD_ADAPTER_INIT ?= \
  -c 'interface sysfsgpio' \
  -c 'sysfsgpio_swd_nums $(SWCLK_PIN) $(SWDIO_PIN)' \
  -c 'sysfsgpio_srst_num $(SRST_PIN)' \
  -c 'adapter_nsrst_delay 100' \
  -c 'adapter_nsrst_assert_width 100'

# default to SWD
OPENOCD_TRANSPORT ?= swd
