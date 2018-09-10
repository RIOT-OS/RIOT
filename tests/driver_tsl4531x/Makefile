BOARD ?= stm32f4discovery

include ../Makefile.tests_common

USEMODULE += tsl4531x
USEMODULE += xtimer

I2C_PORT ?= 0

CFLAGS += -DTSL4531_I2C_PORT=$(I2C_PORT)

include $(RIOTBASE)/Makefile.include
