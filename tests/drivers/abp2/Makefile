include ../Makefile.drivers_common

# include and auto-initialize all available sensors
USEMODULE += saul_default

# use abp2_spi for SPI-Mode and abp2_i2c for I2C-Mode
USEMODULE += abp2
ABP2_INTERFACE ?= abp2_spi
USEMODULE += $(ABP2_INTERFACE)

USEMODULE += ztimer
USEMODULE += ztimer_usec
USEMODULE += phydat

include $(RIOTBASE)/Makefile.include
