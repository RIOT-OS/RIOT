# Raspberry Pi debug adapter over sysfs gpio's
SWDCLK_PIN ?= 21
SWDIO_PIN ?= 20
SRST_PIN ?= 16

OPENOCD_ADAPTER_INIT ?= \
  -c 'source [find interface/sysfsgpio-raspberrypi.cfg]'
  -c 'transport select swd' \
  -c 'adapter_nsrst_assert_width 100'
  -c 'sysfsgpio_swd_nums $(SWDCLK_PIN) $(SWDIO_PIN)'\
  -c 'sysfsgpio_srst_num $(SRST_PIN)'
export OPENOCD_ADAPTER_INIT

