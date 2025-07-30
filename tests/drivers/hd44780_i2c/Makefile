include ../Makefile.drivers_common

USEMODULE += hd44780

# The I2C versions of this display usually use an I2C port expander
# such as the PCF8574, PCF8574A or PCF8575. Depending on the
# board variant, the port expander can also have different
# I2C addresses. Typical ranges are:
#   PCF8574  - 0x20 to 0x27
#   PCF8574A - 0x38 to 0x3F
#   PCF8475  - 0x20 to 0x27
DRIVER ?= pcf8574
ADDRESS ?= 0x20

# select the right module and calculate the I2C address offset
ifeq (pcf8574,$(DRIVER))
  CFLAGS += -D"PCF857X_PARAM_ADDR=($(ADDRESS)-PCF8574_BASE_ADDR)"
else ifeq (pcf8574a,$(DRIVER))
  CFLAGS += -D"PCF857X_PARAM_ADDR=($(ADDRESS)-PCF8574A_BASE_ADDR)"
else ifeq (pcf8575,$(DRIVER))
  CFLAGS += -D"PCF857X_PARAM_ADDR=($(ADDRESS)-PCF8575_BASE_ADDR)"
else
  $(error Invalid I2C port expander selected: $(DRIVER))
endif

USEMODULE += $(DRIVER)

include $(RIOTBASE)/Makefile.include
