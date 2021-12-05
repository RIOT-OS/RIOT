BOARD ?= stm32f4discovery
include ../Makefile.drivers_common

# Only build on a subset of boards (one per arch supported and
# with enough features provided)
BOARD_WHITELIST ?= \
    atmega256rfr2-xpro \
    esp32-wroom-32 \
    stm32f4discovery \
    #

# Get the list of drivers with a SAUL interface (without saul periph)
DRIVERS_DIR = $(RIOTBASE)/drivers
DRIVERS_WITH_SAUL_PATHS = $(filter-out $(wildcard $(DRIVERS_DIR)/saul/*_saul.c),$(wildcard $(DRIVERS_DIR)/*/*_saul.c))
DRIVERS ?= $(subst _saul.c,,$(notdir $(DRIVERS_WITH_SAUL_PATHS)))
USEMODULE += $(DRIVERS)

# Somes drivers with submodules needs special care to select a precise driver variant
ifneq (,$(filter adcxx1c,$(DRIVERS)))
  USEMODULE += adc081c
endif
ifneq (,$(filter apds99xx,$(DRIVERS)))
  USEMODULE += apds9960
endif
ifneq (,$(filter bme680,$(DRIVERS)))
  USEMODULE += bme680_i2c
endif
ifneq (,$(filter lm75,$(DRIVERS)))
  USEMODULE += tmp1075
endif
ifneq (,$(filter lsm6dsxx,$(DRIVERS)))
  USEMODULE += lsm6dsl
endif
ifneq (,$(filter lpsxxx,$(DRIVERS)))
  USEMODULE += lps22hh
endif
ifneq (,$(filter mcp23x17,$(DRIVERS)))
  USEMODULE += mcp23x17_i2c
endif
ifneq (,$(filter mhz19,$(DRIVERS)))
  USEMODULE += mhz19_uart
endif
ifneq (,$(filter mpu9x50,$(DRIVERS)))
  USEMODULE += mpu9150
endif
ifneq (,$(filter pcf857x,$(DRIVERS)))
  USEMODULE += pcf8575
endif
ifneq (,$(filter si70xx,$(DRIVERS)))
  USEMODULE += si7021
endif
ifneq (,$(filter tmp00x,$(DRIVERS)))
  USEMODULE += tmp006
endif
ifneq (,$(filter vl6180x,$(DRIVERS)))
  USEMODULE += vl6180x_als
  USEMODULE += vl6180x_rng
endif

USEMODULE += saul_default

include $(RIOTBASE)/Makefile.include
