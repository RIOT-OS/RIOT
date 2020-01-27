include ../Makefile.tests_common

DRIVER ?= lps331ap

USEMODULE += $(DRIVER)
USEMODULE += xtimer

ifdef CONFIG_LPSXXX_SPI
USEMODULE += periph_spi
endif

ifdef CONFIG_LPSXXX_I2C
USEMODULE += periph_i2c
endif

include $(RIOTBASE)/Makefile.include
