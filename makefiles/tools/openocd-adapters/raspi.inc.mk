# Raspberry Pi GPIO as debug adapter

# Numbers are GPIO numbers
SWCLK_PIN ?= 21
SWDIO_PIN ?= 20
SRST_PIN  ?= 16

ifeq (1, $(shell grep ARMv6 /proc/cpuinfo > /dev/null; echo $$?))
  ifeq (0, $(shell grep [[:lower:]]0311[[:digit:]] /proc/cpuinfo > /dev/null; echo $$?))
    # raspi4
    PERIPH_BASE  = 0xFE000000
    SPEED_COEFF  = 236181
    SPEED_OFFSET = 60
  else
    # raspi2, 3
    PERIPH_BASE  = 0x3F000000
    SPEED_COEFF  = 146203
    SPEED_OFFSET = 36
  endif
else
    # raspi1
    PERIPH_BASE  = 0x20000000
    SPEED_COEFF  = 113714
    SPEED_OFFSET = 28
endif

OPENOCD_ADAPTER_INIT ?= \
  -c 'adapter driver bcm2835gpio' \
  -c 'bcm2835gpio_peripheral_base $(PERIPH_BASE)' \
  -c 'bcm2835gpio_speed_coeffs $(SPEED_COEFF) $(SPEED_OFFSET)' \
  -c 'bcm2835gpio_swd_nums $(SWCLK_PIN) $(SWDIO_PIN)' \
  -c 'bcm2835gpio_srst_num $(SRST_PIN)'

# Default to SWD
OPENOCD_TRANSPORT ?= swd

# bcm2835gpio needs access to /dev/mem
export OPENOCD ?= sudo -E openocd
