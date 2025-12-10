MODULE = u8g2_disp_dev

# Check that either I2C or SPI are there (only supported peripherals)
SUPPORTED_PERIPHERALS := periph_i2c periph_spi
ifeq (,$(filter $(SUPPORTED_PERIPHERALS),$(USEMODULE)))
  $(info u8g2_disp_dev requires either I2C or SPI to work.)
  $(error Please check your display and correspondingly add any of $(SUPPORTED_PERIPHERALS))
endif

include $(RIOTBASE)/Makefile.base
